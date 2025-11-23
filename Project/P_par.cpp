#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <tuple>
#include <fstream>
#include <iomanip>
#include <omp.h>

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

    int num_threads = omp_get_max_threads();
    // main simulation
    for (int gen = 0; gen < P.N_GEN; ++gen) {
        // ---------- RABBITS PHASE ----------
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

        int T;
        #pragma omp parallel
        {
            #pragma omp single
            T = omp_get_num_threads();
        }
        if (T < 1) T = 1;

        vector< unordered_map<int, vector<int>> > prop_rabbit_local(T);
        vector< vector<pair<int,int>> > rabbit_newpos_local(T);
        vector< vector<Animal> > rabbit_newborns_local(T);
        vector< vector<int> > rabbit_ids_local(T);

        // Partition animals by rows
        auto row_range_for_thread = [&](int tid)->pair<int,int>{
            int rows = P.R;
            int base = (rows / T);
            int rem = rows % T;
            int rstart = tid * base + min(tid, rem);
            int rend = rstart + base - 1 + (tid < rem ? 1 : 0);
            if (rstart < 0) rstart = 0;
            if (rend >= rows) rend = rows-1;
            return make_pair(rstart,rend);
        };

        for (int tid = 0; tid < T; ++tid) {
            rabbit_ids_local[tid].clear();
        }
        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive || animals[id].type != 0) continue;
            int x = animals[id].x;
            int tid = (int)((long long)x * T / P.R);
            if (tid < 0) tid = 0;
            if (tid >= T) tid = T-1;
            rabbit_ids_local[tid].push_back(id);
        }

        for (int tid = 0; tid < T; ++tid) {
            rabbit_newpos_local[tid].assign(next_id, make_pair(-1,-1));
        }

        #pragma omp parallel num_threads(T)
        {
            int tid = omp_get_thread_num();
            auto &local_prop = prop_rabbit_local[tid];
            auto &local_newborns = rabbit_newborns_local[tid];
            auto &local_newpos = rabbit_newpos_local[tid];
            auto &ids = rabbit_ids_local[tid];

            for (int idx = 0; idx < (int)ids.size(); ++idx) {
                int id = ids[idx];
                if (!animals[id].alive) continue;
                int x = animals[id].x, y = animals[id].y;
                vector<pair<int,int>> opts;
                opts.reserve(4);
                for (int d = 0; d < 4; ++d) {
                    int nx = x + DX[d], ny = y + DY[d];
                    if (!inside(nx,ny,P.R,P.C)) continue;
                    if (grid_after_rabbits[nx][ny] == EMPTY_CELL) opts.emplace_back(nx,ny);
                }
                if (opts.empty()) {
                    local_newpos[id] = make_pair(x,y);
                    continue;
                }
                int idx_choice = choose_idx(gen, x, y, opts);
                int tx = opts[idx_choice].first, ty = opts[idx_choice].second;
                int key = tx * P.C + ty;
                local_prop[key].push_back(id);
            }  
        } 

        unordered_map<int, vector<int>> prop_rabbit;
        prop_rabbit.reserve(1024);
        for (int tid = 0; tid < T; ++tid) {
            for (auto &kv : prop_rabbit_local[tid]) {
                int key = kv.first;
                auto &vec = kv.second;
                auto &dest = prop_rabbit[key]; 
                dest.insert(dest.end(), vec.begin(), vec.end());
            }
        }

        vector<pair<int,int>> rabbit_newpos(next_id, make_pair(-1,-1));
        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive || animals[id].type != 0) continue;
            rabbit_newpos[id] = make_pair(-1,-1);
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

        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive || animals[id].type != 0) continue;
            if (rabbit_newpos[id].first == -1) {
                int x = animals[id].x, y = animals[id].y;
                vector<pair<int,int>> opts;
                opts.reserve(4);
                for (int d = 0; d < 4; ++d) {
                    int nx = x + DX[d], ny = y + DY[d];
                    if (!inside(nx,ny,P.R,P.C)) continue;
                    if (grid_after_rabbits[nx][ny] == EMPTY_CELL) opts.emplace_back(nx,ny);
                }
                if (opts.empty()) {
                    rabbit_newpos[id] = make_pair(x,y);
                } else {
                    if (!animals[id].alive) {
                        rabbit_newpos[id] = make_pair(-1,-1);
                    } else {
                        rabbit_newpos[id] = make_pair(x,y);
                    }
                }
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
        for (int i = 0; i < P.R; ++i) for (int j = 0; j < P.C; ++j)
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
                if (inside(x,y,P.R,P.C)) {
                    grid_post_rabbits[x][y] = id; 
                }
            }
        }
        grid = grid_post_rabbits;

        // ---------- FOXES PHASE ----------
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

        vector< unordered_map<int, vector<int>> > prop_fox_eat_local(T);
        vector< unordered_map<int, vector<int>> > prop_fox_empty_local(T);
        vector< vector<pair<int,int>> > fox_newpos_local(T, vector<pair<int,int>>(next_id, make_pair(-1,-1)));
        vector<int> fox_ids; fox_ids.reserve(1024);
        vector<vector<int>> fox_ids_local(T);
        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive || animals[id].type != 1) continue;
            int x = animals[id].x;
            int tid = (int)((long long)x * T / P.R);
            if (tid < 0) tid = 0;
            if (tid >= T) tid = T-1;
            fox_ids_local[tid].push_back(id);
        }

        #pragma omp parallel num_threads(T)
        {
            int tid = omp_get_thread_num();
            auto &local_eat = prop_fox_eat_local[tid];
            auto &local_empty = prop_fox_empty_local[tid];
            auto &ids = fox_ids_local[tid];

            for (int idx = 0; idx < (int)ids.size(); ++idx) {
                int id = ids[idx];
                if (!animals[id].alive) continue;
                int x = animals[id].x, y = animals[id].y;
                vector<pair<int,int>> rabbits_adj;
                vector<pair<int,int>> empties_adj;
                for (int d = 0; d < 4; ++d) {
                    int nx = x + DX[d], ny = y + DY[d];
                    if (!inside(nx,ny,P.R,P.C)) continue;
                    int g = grid[nx][ny];
                    if (g >= 0 && animals[g].alive && animals[g].type == 0) {
                        rabbits_adj.emplace_back(nx,ny);
                    } else if (grid_after_foxs[nx][ny] == EMPTY_CELL) {
                        empties_adj.emplace_back(nx,ny);
                    }
                }
                if (!rabbits_adj.empty()) {
                    int idx_choice = choose_idx(gen, x, y, rabbits_adj);
                    int tx = rabbits_adj[idx_choice].first, ty = rabbits_adj[idx_choice].second;
                    int key = tx * P.C + ty;
                    local_eat[key].push_back(id);
                    continue;
                }
                if (animals[id].since_ate >= P.GEN_FOOD_FOXES) {
                    continue;
                }
                if (empties_adj.empty()) {
                    continue;
                }
                int idx_choice = choose_idx(gen, x, y, empties_adj);
                int tx = empties_adj[idx_choice].first, ty = empties_adj[idx_choice].second;
                int key = tx * P.C + ty;
                local_empty[key].push_back(id);
            }
        } 

        unordered_map<int, vector<int>> prop_fox_eat;
        unordered_map<int, vector<int>> prop_fox_empty;
        prop_fox_eat.reserve(1024);
        prop_fox_empty.reserve(1024);
        for (int tid = 0; tid < T; ++tid) {
            for (auto &kv : prop_fox_eat_local[tid]) {
                int key = kv.first;
                auto &dest = prop_fox_eat[key];
                dest.insert(dest.end(), kv.second.begin(), kv.second.end());
            }
            for (auto &kv : prop_fox_empty_local[tid]) {
                int key = kv.first;
                auto &dest = prop_fox_empty[key];
                dest.insert(dest.end(), kv.second.begin(), kv.second.end());
            }
        }

        vector<char> fox_ate_flag(next_id, 0);
        vector<char> will_die(next_id, 0);

        for (auto &kv : prop_fox_eat) {
            int key = kv.first;
            vector<int> &vec = kv.second;
            if (vec.empty()) continue;
            int tx = key / P.C, ty = key % P.C;
            int victim_id = grid[tx][ty];
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
            fox_ate_flag[best] = 1;
            for (int fid : vec) {
                if (fid == best) continue;
                animals[fid].alive = false;
                pos_of_id[fid] = make_pair(-1,-1);
            }
            animals[victim_id].alive = false;
            pos_of_id[victim_id] = make_pair(-1,-1);
            animals[best].x = animals[best].x; 
        }

        
        unordered_map<int, pair<int,int>> fox_newpos_map; 

        
        for (auto &kv : prop_fox_eat) {
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
            int victim_id = grid[tx][ty];

            fox_newpos_map[best] = make_pair(tx,ty);
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
            fox_newpos_map[best] = make_pair(tx,ty);
            for (int fid : vec) {
                if (fid == best) continue;
                animals[fid].alive = false;
                pos_of_id[fid] = make_pair(-1,-1);
            }
        }

        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive || animals[id].type != 1) continue;
            if (fox_newpos_map.find(id) == fox_newpos_map.end()) {
                if (animals[id].since_ate >= P.GEN_FOOD_FOXES) {
                    animals[id].alive = false;
                    pos_of_id[id] = make_pair(-1,-1);
                } else {
                    fox_newpos_map[id] = make_pair(animals[id].x, animals[id].y);
                }
            }
        }

        vector<Animal> fox_newborns;
        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive || animals[id].type != 1) continue;
            pair<int,int> np = fox_newpos_map[id];
            if (np.first == -1) {
                animals[id].alive = false;
                pos_of_id[id] = make_pair(-1,-1);
                continue;
            }
            int oldx = animals[id].x, oldy = animals[id].y;
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
        #pragma omp parallel for schedule(static)
        for (int id = 0; id < (int)animals.size(); ++id) {
            if (!animals[id].alive) continue;
            int x = animals[id].x, y = animals[id].y;
            if (!inside(x,y,P.R,P.C)) continue;
            #pragma omp critical
            {
                if (grid_next[x][y] == EMPTY_CELL) {
                    grid_next[x][y] = id;
                } else {
                    animals[id].alive = false;
                    pos_of_id[id] = make_pair(-1,-1);
                }
            }
        }

        grid.swap(grid_next);

        print_generation_to_file(viz_out, grid, animals, gen + 1);
    }

    viz_out.close();

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

    return 0;
}
