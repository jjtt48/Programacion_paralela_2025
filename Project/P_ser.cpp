#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <tuple>
#include <fstream>
#include <iomanip>

using namespace std;

enum CellType { EMPTY_CELL = -1, ROCK_CELL = -2 };

struct Animal {
    int id;
    int x, y;
    int type; 
    int repro_since;
    int since_ate; 
    bool alive;
    Animal() : id(-1), x(-1), y(-1), type(0), repro_since(0), since_ate(0), alive(false) {}
    Animal(int _id, int _x, int _y, int _type)
        : id(_id), x(_x), y(_y), type(_type), repro_since(0), since_ate(0), alive(true) {}
};

struct Params {
    int GEN_PROC_RABBITS;
    int GEN_PROC_FOXES;
    int GEN_FOOD_FOXES;
    int N_GEN;
    int R, C;
};

static const int DX[4] = {-1, 0, 1, 0}; // N, E, S, W
static const int DY[4] = {0, 1, 0, -1};

inline bool inside(int x, int y, int R, int C) {
    return x >= 0 && x < R && y >= 0 && y < C;
}


void print_generation_to_file(ofstream &out, const vector<vector<int>> &grid,
                              const vector<Animal> &animals, int gen_num) {
    int R = (int)grid.size();
    int C = (int)grid[0].size();
    out << "Gen " << gen_num << "\n";
    int dash_count = C + 2; 
    for (int i = 0; i < dash_count; ++i) out << "-";
    out << "\n";
    for (int i = 0; i < R; ++i) {
        out << "|";
        for (int j = 0; j < C; ++j) {
            char ch = ' ';
            int g = grid[i][j];
            if (g == ROCK_CELL) ch = '*';
            else if (g >= 0) {
                if (g < (int)animals.size() && animals[g].alive) {
                    if (animals[g].type == 0) ch = 'R';
                    else ch = 'F';
                } else {
                    ch = ' ';
                }
            } else ch = ' ';
            out << ch;
        }
        out << "|\n";
    }
    for (int i = 0; i < dash_count; ++i) out << "-";
    out << "\n\n";
    out.flush();
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Params P;
    int N_objects;
    if (!(cin >> P.GEN_PROC_RABBITS >> P.GEN_PROC_FOXES >> P.GEN_FOOD_FOXES
          >> P.N_GEN >> P.R >> P.C >> N_objects)) {
        cerr << "Formato de entrada incorrecto\n";
        return 1;
    }

    vector<vector<int>> grid(P.R, vector<int>(P.C, EMPTY_CELL));
    vector<Animal> animals;
    animals.reserve(100000);
    vector<pair<int,int>> pos_of_id; 

    int next_id = 0;
    for (int i = 0; i < N_objects; ++i) {
        string obj;
        int x, y;
        cin >> obj >> x >> y;
        if (!inside(x,y,P.R,P.C)) {
            cerr << "Objeto fuera de bounds\n";
            return 1;
        }
        if (obj == "ROCK") {
            grid[x][y] = ROCK_CELL;
        } else if (obj == "RABBIT") {
            animals.push_back(Animal(next_id, x, y, 0));
            pos_of_id.push_back(make_pair(x,y));
            grid[x][y] = next_id;
            ++next_id;
        } else if (obj == "FOX") {
            animals.push_back(Animal(next_id, x, y, 1));
            animals.back().since_ate = 0;
            pos_of_id.push_back(make_pair(x,y));
            grid[x][y] = next_id;
            ++next_id;
        } else {
            cerr << "Objeto desconocido: " << obj << "\n";
            return 1;
        }
    }

    ofstream viz_out("visual.txt");
    if (!viz_out.is_open()) {
        cerr << "No se pudo crear visual.txt\n";
        return 1;
    }

    print_generation_to_file(viz_out, grid, animals, 0);

    auto choose_idx = [&](int gen, int x, int y, const vector<pair<int,int>>& opts) -> int {
        if (opts.empty()) return -1;
        int p = (int)opts.size();
        return (gen + x + y) % p;
    };

    // Simulación
    for (int gen = 0; gen < P.N_GEN; ++gen) {
        // -------- RABBITS PHASE --------
        vector<vector<int>> grid_after_rabbits = grid;
        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive) continue;
            if (animals[id].type == 0) {
                int ox = animals[id].x, oy = animals[id].y;
                if (inside(ox,oy,P.R,P.C) && grid_after_rabbits[ox][oy] == id) {
                    grid_after_rabbits[ox][oy] = EMPTY_CELL;
                }
            }
        }

        unordered_map<int, vector<int>> prop_rabbit;
        prop_rabbit.reserve(1024);

        vector<pair<int,int>> rabbit_newpos(next_id, make_pair(-1,-1));
        vector<bool> rabbit_proposed(next_id, false);

        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive || animals[id].type != 0) continue;
            int x = animals[id].x, y = animals[id].y;
            vector<pair<int,int>> opts;
            for (int d = 0; d < 4; ++d) {
                int nx = x + DX[d], ny = y + DY[d];
                if (!inside(nx,ny,P.R,P.C)) continue;
                if (grid_after_rabbits[nx][ny] == EMPTY_CELL) opts.push_back(make_pair(nx,ny));
            }
            if (opts.empty()) {
                rabbit_newpos[id] = make_pair(x,y);
                continue;
            }
            int idx = choose_idx(gen, x, y, opts);
            int tx = opts[idx].first, ty = opts[idx].second;
            int key = tx * P.C + ty;
            prop_rabbit[key].push_back(id);
            rabbit_proposed[id] = true;
        }

        for (auto &kv : prop_rabbit) {
            int key = kv.first;
            vector<int> &vec = kv.second;
            if (vec.empty()) continue;
            int best = vec[0];
            for (size_t i = 1; i < vec.size(); ++i) {
                int id = vec[i];
                if (animals[id].repro_since > animals[best].repro_since) best = id;
                else if (animals[id].repro_since == animals[best].repro_since && id < best) best = id;
            }
            int tx = key / P.C, ty = key % P.C;
            rabbit_newpos[best] = make_pair(tx,ty);
            for (size_t i = 0; i < vec.size(); ++i) {
                int id = vec[i];
                if (id == best) continue;
                animals[id].alive = false;
                pos_of_id[id] = make_pair(-1,-1);
            }
        }

        vector<Animal> rabbit_newborns;
        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive || animals[id].type != 0) continue;
            int oldx = animals[id].x, oldy = animals[id].y;
            pair<int,int> np = rabbit_newpos[id];
            if (np.first == -1) {
                animals[id].alive = false;
                pos_of_id[id] = make_pair(-1,-1);
                continue;
            }
            int nx = np.first, ny = np.second;
            bool moved = !(nx == oldx && ny == oldy);
            if (moved && animals[id].repro_since >= P.GEN_PROC_RABBITS) {
                Animal child(next_id, oldx, oldy, 0);
                child.repro_since = 0;
                child.since_ate = 0;
                rabbit_newborns.push_back(child);
                pos_of_id.push_back(make_pair(oldx,oldy));
                ++next_id;
                animals[id].repro_since = 0;
            } else {
                animals[id].repro_since++;
            }
            animals[id].x = nx;
            animals[id].y = ny;
            pos_of_id[id] = make_pair(nx,ny);
        }

        for (size_t i = 0; i < rabbit_newborns.size(); ++i) {
            Animal &ch = rabbit_newborns[i];
            if (grid_after_rabbits[ch.x][ch.y] == EMPTY_CELL) {
                animals.push_back(ch);
                grid_after_rabbits[ch.x][ch.y] = ch.id;
            } else {
                pos_of_id[ch.id] = make_pair(-1,-1);
                animals[ch.id].alive = false;
            }
        }

        vector<vector<int>> grid_post_rabbits(P.R, vector<int>(P.C, EMPTY_CELL));
        for (int i = 0; i < P.R; ++i)
            for (int j = 0; j < P.C; ++j)
                if (grid[i][j] == ROCK_CELL) grid_post_rabbits[i][j] = ROCK_CELL;
        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive) continue;
            if (animals[id].type == 0) {
                int x = animals[id].x, y = animals[id].y;
                if (inside(x,y,P.R,P.C) && grid_post_rabbits[x][y] == EMPTY_CELL) {
                    grid_post_rabbits[x][y] = id;
                } else {
                    animals[id].alive = false;
                    pos_of_id[id] = make_pair(-1,-1);
                }
            }
        }
        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive) continue;
            if (animals[id].type == 1) {
                int x = animals[id].x, y = animals[id].y;
                if (inside(x,y,P.R,P.C) && grid_post_rabbits[x][y] == EMPTY_CELL) {
                    grid_post_rabbits[x][y] = id;
                } else {
                    grid_post_rabbits[x][y] = id;
                }
            }
        }

        grid = grid_post_rabbits;

        // -------- FOXES PHASE --------
        vector<vector<int>> grid_after_foxs = grid;
        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive) continue;
            if (animals[id].type == 1) {
                int ox = animals[id].x, oy = animals[id].y;
                if (inside(ox,oy,P.R,P.C) && grid_after_foxs[ox][oy] == id) {
                    grid_after_foxs[ox][oy] = EMPTY_CELL;
                }
            }
        }

        unordered_map<int, vector<int>> prop_fox_eat;
        unordered_map<int, vector<int>> prop_fox_empty;
        vector<pair<int,int>> fox_newpos(next_id, make_pair(-1,-1));
        vector<bool> fox_proposed(next_id, false);
        vector<bool> will_die_starvation(next_id, false);

        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive || animals[id].type != 1) continue;
            int x = animals[id].x, y = animals[id].y;
            vector<pair<int,int>> rabbits_adj;
            vector<pair<int,int>> empties_adj;
            for (int d = 0; d < 4; ++d) {
                int nx = x + DX[d], ny = y + DY[d];
                if (!inside(nx,ny,P.R,P.C)) continue;
                int g = grid[nx][ny];
                if (g >= 0 && animals[g].alive && animals[g].type == 0) {
                    rabbits_adj.push_back(make_pair(nx,ny));
                } else if (grid_after_foxs[nx][ny] == EMPTY_CELL) {
                    empties_adj.push_back(make_pair(nx,ny));
                }
            }
            if (!rabbits_adj.empty()) {
                int idx = choose_idx(gen, x, y, rabbits_adj);
                int tx = rabbits_adj[idx].first, ty = rabbits_adj[idx].second;
                int key = tx * P.C + ty;
                prop_fox_eat[key].push_back(id);
                fox_proposed[id] = true;
                continue;
            }
            if (animals[id].since_ate >= P.GEN_FOOD_FOXES) {
                will_die_starvation[id] = true;
                animals[id].alive = false;
                pos_of_id[id] = make_pair(-1,-1);
                continue;
            }
            if (empties_adj.empty()) {
                fox_newpos[id] = make_pair(x,y);
                continue;
            }
            int idx = choose_idx(gen, x, y, empties_adj);
            int tx = empties_adj[idx].first, ty = empties_adj[idx].second;
            int key = tx * P.C + ty;
            prop_fox_empty[key].push_back(id);
            fox_proposed[id] = true;
        }

        vector<char> fox_ate_flag(next_id, 0);
        for (auto &kv : prop_fox_eat) {
            int key = kv.first;
            vector<int> &vec = kv.second;
            if (vec.empty()) continue;
            int tx = key / P.C, ty = key % P.C;
            int victim_id = grid[tx][ty];
            if (!(victim_id >= 0 && animals[victim_id].alive && animals[victim_id].type == 0)) {
                for (size_t i = 0; i < vec.size(); ++i) {
                    int fid = vec[i];
                    if (!animals[fid].alive) continue;
                    int fx = animals[fid].x, fy = animals[fid].y;
                    vector<pair<int,int>> empties_adj;
                    for (int d = 0; d < 4; ++d) {
                        int nx = fx + DX[d], ny = fy + DY[d];
                        if (!inside(nx,ny,P.R,P.C)) continue;
                        if (grid_after_foxs[nx][ny] == EMPTY_CELL) empties_adj.push_back(make_pair(nx,ny));
                    }
                    if (empties_adj.empty()) {
                        fox_newpos[fid] = make_pair(fx,fy);
                    } else {
                        int idx = choose_idx(gen, fx, fy, empties_adj);
                        int tx2 = empties_adj[idx].first, ty2 = empties_adj[idx].second;
                        int key2 = tx2 * P.C + ty2;
                        prop_fox_empty[key2].push_back(fid);
                    }
                }
                continue;
            }
            int best = vec[0];
            for (size_t i = 1; i < vec.size(); ++i) {
                int fid = vec[i];
                if (!animals[fid].alive) continue;
                if (animals[fid].repro_since > animals[best].repro_since) best = fid;
                else if (animals[fid].repro_since == animals[best].repro_since) {
                    int rem_best = P.GEN_FOOD_FOXES - animals[best].since_ate;
                    int rem_fid = P.GEN_FOOD_FOXES - animals[fid].since_ate;
                    if (rem_fid > rem_best) best = fid;
                    else if (rem_fid == rem_best && fid < best) best = fid;
                }
            }
            fox_newpos[best] = make_pair(tx,ty);
            fox_ate_flag[best] = 1;
            animals[victim_id].alive = false;
            pos_of_id[victim_id] = make_pair(-1,-1);
            for (size_t i = 0; i < vec.size(); ++i) {
                int fid = vec[i];
                if (fid == best) continue;
                animals[fid].alive = false;
                pos_of_id[fid] = make_pair(-1,-1);
            }
        }

        for (auto &kv : prop_fox_empty) {
            int key = kv.first;
            vector<int> &vec = kv.second;
            if (vec.empty()) continue;
            int tx = key / P.C, ty = key % P.C;
            int best = vec[0];
            for (size_t i = 1; i < vec.size(); ++i) {
                int fid = vec[i];
                if (!animals[fid].alive) continue;
                if (animals[fid].repro_since > animals[best].repro_since) best = fid;
                else if (animals[fid].repro_since == animals[best].repro_since) {
                    int rem_best = P.GEN_FOOD_FOXES - animals[best].since_ate;
                    int rem_fid = P.GEN_FOOD_FOXES - animals[fid].since_ate;
                    if (rem_fid > rem_best) best = fid;
                    else if (rem_fid == rem_best && fid < best) best = fid;
                }
            }
            fox_newpos[best] = make_pair(tx,ty);
            for (size_t i = 0; i < vec.size(); ++i) {
                int fid = vec[i];
                if (fid == best) continue;
                animals[fid].alive = false;
                pos_of_id[fid] = make_pair(-1,-1);
            }
        }

        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive || animals[id].type != 1) continue;
            if (fox_newpos[id].first == -1) {
                if (will_die_starvation[id]) continue;
                fox_newpos[id] = make_pair(animals[id].x, animals[id].y);
            }
        }

        vector<Animal> fox_newborns;
        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive || animals[id].type != 1) continue;
            int oldx = animals[id].x, oldy = animals[id].y;
            pair<int,int> np = fox_newpos[id];
            if (np.first == -1) {
                animals[id].alive = false;
                pos_of_id[id] = make_pair(-1,-1);
                continue;
            }
            int nx = np.first, ny = np.second;
            bool moved = !(nx == oldx && ny == oldy);
            bool ate = (fox_ate_flag[id] != 0);
            if (ate) animals[id].since_ate = 0;
            else animals[id].since_ate++;
            if (moved && animals[id].repro_since >= P.GEN_PROC_FOXES) {
                Animal child(next_id, oldx, oldy, 1);
                child.repro_since = 0;
                child.since_ate = 0;
                fox_newborns.push_back(child);
                pos_of_id.push_back(make_pair(oldx,oldy));
                ++next_id;
                animals[id].repro_since = 0;
            } else {
                animals[id].repro_since++;
            }
            animals[id].x = nx;
            animals[id].y = ny;
            pos_of_id[id] = make_pair(nx,ny);
        }

        for (size_t i = 0; i < fox_newborns.size(); ++i) {
            Animal &ch = fox_newborns[i];
            int cx = ch.x, cy = ch.y;
            if (inside(cx,cy,P.R,P.C) && grid_after_foxs[cx][cy] == EMPTY_CELL) {
                animals.push_back(ch);
                grid_after_foxs[cx][cy] = ch.id;
            } else {
                pos_of_id[ch.id] = make_pair(-1,-1);
                animals[ch.id].alive = false;
            }
        }

        vector<vector<int>> grid_next(P.R, vector<int>(P.C, EMPTY_CELL));
        for (int i = 0; i < P.R; ++i)
            for (int j = 0; j < P.C; ++j)
                if (grid_post_rabbits[i][j] == ROCK_CELL) grid_next[i][j] = ROCK_CELL;
        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive) continue;
            int x = animals[id].x, y = animals[id].y;
            if (!inside(x,y,P.R,P.C)) continue;
            if (grid_next[x][y] == EMPTY_CELL) {
                grid_next[x][y] = id;
            } else {
                animals[id].alive = false;
                pos_of_id[id] = make_pair(-1,-1);
            }
        }

        grid = grid_next;

        print_generation_to_file(viz_out, grid, animals, gen + 1);
    }

    vector< tuple<string,int,int> > final_objs;
    for (int i = 0; i < P.R; ++i) {
        for (int j = 0; j < P.C; ++j) {
            if (grid[i][j] == ROCK_CELL) final_objs.emplace_back("ROCK", i, j);
            else if (grid[i][j] >= 0) {
                int id = grid[i][j];
                if (id >= 0 && id < (int)animals.size() && animals[id].alive) {
                    if (animals[id].type == 0) final_objs.emplace_back("RABBIT", i, j);
                    else final_objs.emplace_back("FOX", i, j);
                }
            }
        }
    }

    cout << P.GEN_PROC_RABBITS << " " << P.GEN_PROC_FOXES << " " << P.GEN_FOOD_FOXES
         << " " << 0 << " " << P.R << " " << P.C << " " << (int)final_objs.size() << "\n";
    for (size_t i = 0; i < final_objs.size(); ++i) {
        cout << get<0>(final_objs[i]) << " " << get<1>(final_objs[i]) << " " << get<2>(final_objs[i]) << "\n";
    }

    viz_out.close();
    return 0;
}
