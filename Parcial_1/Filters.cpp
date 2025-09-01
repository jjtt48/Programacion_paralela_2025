#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>

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

class Filter {
private:
    float kernel[3][3];
public:
    Filter(const char* type) {
        if (strcmp(type,"blur")==0) {
            const float k[3][3] = {
                {1.0f/9, 1.0f/9, 1.0f/9},
                {1.0f/9, 1.0f/9, 1.0f/9},
                {1.0f/9, 1.0f/9, 1.0f/9}
            };
            memcpy(kernel, k, sizeof(kernel));
        } 
        else if (strcmp(type,"laplace")==0) {
            const float k[3][3] = {
                { 0, -1,  0},
                {-1,  4, -1},
                { 0, -1,  0}
            };
            memcpy(kernel, k, sizeof(kernel));
        } 
        else if (strcmp(type,"sharpen")==0) {
            const float k[3][3] = {
                { 0, -1,  0},
                {-1,  5, -1},
                { 0, -1,  0}
            };
            memcpy(kernel, k, sizeof(kernel));
        } 
        else {
            
            memset(kernel, 0, sizeof(kernel));
        }
    }
    void apply(Image* img) {
        int w = img->getWidth();
        int h = img->getHeight();
        int maxv = img->getMaxval();
        int channels = (strcmp(img->getMagic(),"P3")==0) ? 3 : 1;
        int* input = img->getPixels();
        int* output = (int*)malloc(sizeof(int) * w * h * channels);

        for (int y=0; y<h; y++) {
            for (int x=0; x<w; x++) {
                for (int c=0; c<channels; c++) {
                    float sum = 0.0f;
                    for (int ky=-1; ky<=1; ky++) {
                        for (int kx=-1; kx<=1; kx++) {
                            int nx = x+kx, ny = y+ky;
                            if (nx>=0 && nx<w && ny>=0 && ny<h) {
                                int idx = (ny*w+nx)*channels + c;
                                sum += input[idx] * kernel[ky+1][kx+1];
                            }
                        }
                    }
                    int idx = (y*w+x)*channels + c;
                    int val = (int)sum;
                    output[idx] = std::max(0, std::min(maxv, val));
                }
            }
        }

        img->setPixels(output);
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

    Filter f(argv[4]);  
    f.apply(img);

    FILE* out = fopen(argv[2], "w");
    if (!out) { printf("No se pudo abrir %s\n", argv[2]); delete img; return 1; }

    img->save(out);
    fclose(out);

    delete img;
    return 0;
}
