#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <pthread.h>

class Image {
protected:
    int width, height, maxval;
    char magic[3];
    int* pixels;
public:
    Image(): width(0), height(0), maxval(255), pixels(NULL) {}
    virtual ~Image() { if (pixels) free(pixels); }
    virtual bool load(FILE* in) = 0;
    virtual bool save(FILE* out) = 0;
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getMaxval() const { return maxval; }
    char* getMagic() { return (char*)magic; }
    int* getPixels() { return pixels; }
    void setPixels(int* p) { if (pixels) free(pixels); pixels = p; }
};

class PGMImage : public Image {
public:
    bool load(FILE* in) override {
        fscanf(in, "%2s", magic);
        if (strcmp(magic, "P2") != 0) return false;
        fscanf(in, "%d %d", &width, &height);
        fscanf(in, "%d", &maxval);
        int count = width * height;
        pixels = (int*)malloc(sizeof(int)*count);
        for (int i=0; i<count; i++) fscanf(in, "%d", &pixels[i]);
        return true;
    }
    bool save(FILE* out) override {
        fprintf(out, "P2\n%d %d\n%d\n", width, height, maxval);
        int count = width * height;
        for (int i=0; i<count; i++) fprintf(out, "%d\n", pixels[i]);
        return true;
    }
};

class PPMImage : public Image {
public:
    bool load(FILE* in) override {
        fscanf(in, "%2s", magic);
        if (strcmp(magic, "P3") != 0) return false;
        fscanf(in, "%d %d", &width, &height);
        fscanf(in, "%d", &maxval);
        int count = width * height * 3;
        pixels = (int*)malloc(sizeof(int)*count);
        for (int i=0; i<count; i++) fscanf(in, "%d", &pixels[i]);
        return true;
    }
    bool save(FILE* out) override {
        fprintf(out, "P3\n%d %d\n%d\n", width, height, maxval);
        int count = width * height * 3;
        for (int i=0; i<count; i++) fprintf(out, "%d\n", pixels[i]);
        return true;
    }
};

Image* loadImage(FILE* in) {
    char magic[3];
    fscanf(in, "%2s", magic);
    rewind(in);
    if (strcmp(magic,"P2")==0) {
        PGMImage* img = new PGMImage();
        if (img->load(in)) return img;
        delete img;
    } else if (strcmp(magic,"P3")==0) {
        PPMImage* img = new PPMImage();
        if (img->load(in)) return img;
        delete img;
    }
    return NULL;
}

static int *g_input, *g_output;
static int g_w, g_h, g_channels, g_maxv;
static int midX, midY;
static const float (*g_fkernel)[3];
static const int (*g_ikernel)[3];
static bool g_useFloat;

void* worker(void* arg) {
    int id = *(int*)arg;
    int startX, startY, endX, endY;

    if (id == 0) { startX=0;    startY=0;    endX=midX; endY=midY; }
    else if (id==1){ startX=midX; startY=0;    endX=g_w; endY=midY; }
    else if (id==2){ startX=0;    startY=midY; endX=midX; endY=g_h; }
    else           { startX=midX; startY=midY; endX=g_w; endY=g_h; }

    for (int y=startY; y<endY; y++) {
        for (int x=startX; x<endX; x++) {
            for (int c=0; c<g_channels; c++) {
                float sum = 0.0f;
                for (int ky=-1; ky<=1; ky++) {
                    for (int kx=-1; kx<=1; kx++) {
                        int nx = x+kx, ny = y+ky;
                        if (nx>=0 && nx<g_w && ny>=0 && ny<g_h) {
                            int idx = (ny*g_w+nx)*g_channels + c;
                            if (g_useFloat)
                                sum += g_input[idx] * g_fkernel[ky+1][kx+1];
                            else
                                sum += g_input[idx] * g_ikernel[ky+1][kx+1];
                        }
                    }
                }
                int idx = (y*g_w+x)*g_channels + c;
                int val = (int)sum;
                g_output[idx] = std::max(0, std::min(g_maxv, val));
            }
        }
    }
    return NULL;
}

class Filter {
public:
    explicit Filter(const char* type) {
        if (strcmp(type, "blur") == 0) {
            static const float k[3][3] = {
                {1.0f/9, 1.0f/9, 1.0f/9},
                {1.0f/9, 1.0f/9, 1.0f/9},
                {1.0f/9, 1.0f/9, 1.0f/9}
            };
            g_fkernel = k;
            g_useFloat = true;
        } 
        else if (strcmp(type, "laplace") == 0) {
            static const int k[3][3] = {
                { 0, -1,  0},
                {-1,  4, -1},
                { 0, -1,  0}
            };
            g_ikernel = k;
            g_useFloat = false;
        } 
        else if (strcmp(type, "sharpen") == 0) {
            static const int k[3][3] = {
                { 0, -1,  0},
                {-1,  5, -1},
                { 0, -1,  0}
            };
            g_ikernel = k;
            g_useFloat = false;
        } 
        else {
            
            g_useFloat = true;
            static const float k[3][3] = {{0,0,0},{0,0,0},{0,0,0}};
            g_fkernel = k;
        }
    }

    void apply(Image* img) {
        g_w = img->getWidth();
        g_h = img->getHeight();
        g_maxv = img->getMaxval();
        g_channels = (strcmp(img->getMagic(),"P3")==0) ? 3 : 1;
        g_input = img->getPixels();
        g_output = (int*)malloc(sizeof(int) * g_w * g_h * g_channels);

        midX = g_w/2;
        midY = g_h/2;

        pthread_t threads[4];
        int ids[4] = {0,1,2,3};
        for (int i=0; i<4; i++)
            pthread_create(&threads[i], NULL, worker, &ids[i]);
        for (int i=0; i<4; i++)
            pthread_join(threads[i], NULL);

        img->setPixels(g_output);
    }
};

int main(int argc, char* argv[]) {
    if (argc < 5) {
        printf("input.{pgm|ppm} output.{pgm|ppm} --f {blur|laplace|sharpen}\n");
        return 1;
    }

    FILE* in = fopen(argv[1], "r");
    if (!in) { printf("No se pudo abrir %s\n", argv[1]); return 1; }

    Image* img = loadImage(in);
    fclose(in);
    if (!img) { printf("Archivo no válido\n"); return 1; }

    Filter* f = NULL;
    if (strcmp(argv[3],"--f")==0) {
        f = new Filter(argv[4]);   
    }

    if (!f) {
        printf("Filtro no válido. Opciones: blur, laplace, sharpen\n");
        delete img;
        return 1;
    }

    f->apply(img);

    FILE* out = fopen(argv[2], "w");
    if (!out) { printf("No se pudo abrir %s\n", argv[2]); delete img; delete f; return 1; }

    img->save(out);
    fclose(out);

    delete img;
    delete f;

    return 0;
}

