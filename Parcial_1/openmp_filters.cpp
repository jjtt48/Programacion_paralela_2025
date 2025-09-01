#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <omp.h>

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

enum FilterType { BLUR, LAPLACE, SHARPEN };

class Filter {
    FilterType type;
public:
    Filter(FilterType t): type(t) {}

    void apply(Image* img) {
        int w = img->getWidth();
        int h = img->getHeight();
        int maxv = img->getMaxval();
        int channels = (strcmp(img->getMagic(),"P3")==0) ? 3 : 1;

        int* input = img->getPixels();
        int* output = (int*)malloc(sizeof(int) * w * h * channels);

        float fkernel[3][3];  
        int ikernel[3][3];    
        bool useFloat = false;

        if (type == BLUR) {
            useFloat = true;
            float k[3][3] = {
                {1.0f/9, 1.0f/9, 1.0f/9},
                {1.0f/9, 1.0f/9, 1.0f/9},
                {1.0f/9, 1.0f/9, 1.0f/9}
            };
            memcpy(fkernel, k, sizeof(k));
        } else if (type == LAPLACE) {
            int k[3][3] = {
                { 0, -1,  0},
                {-1,  4, -1},
                { 0, -1,  0}
            };
            memcpy(ikernel, k, sizeof(k));
        } else if (type == SHARPEN) {
            int k[3][3] = {
                { 0, -1,  0},
                {-1,  5, -1},
                { 0, -1,  0}
            };
            memcpy(ikernel, k, sizeof(k));
        }

        omp_set_num_threads(4);
        #pragma omp parallel for
        for (int y=0; y<h; y++) {
            for (int x=0; x<w; x++) {
                for (int c=0; c<channels; c++) {
                    float sum = 0.0f;
                    for (int ky=-1; ky<=1; ky++) {
                        for (int kx=-1; kx<=1; kx++) {
                            int nx = x+kx, ny = y+ky;
                            if (nx>=0 && nx<w && ny>=0 && ny<h) {
                                int idx = (ny*w+nx)*channels + c;
                                if (useFloat) sum += input[idx] * fkernel[ky+1][kx+1];
                                else sum += input[idx] * ikernel[ky+1][kx+1];
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
    if (argc < 3) {
        printf("input.{pgm|ppm} output_base\n");
        return 1;
    }
    const char* dot = strrchr(argv[2], '.');
    char baseName[256];
    strncpy(baseName, argv[2], dot - argv[2]);
    baseName[dot - argv[2]] = '\0';

    char ext[16];
    strcpy(ext, dot);

    char outBlur[300], outLaplace[300], outSharpen[300];
    sprintf(outBlur,    "%s_blur%s", baseName, ext);
    sprintf(outLaplace, "%s_laplace%s", baseName, ext);
    sprintf(outSharpen, "%s_sharpen%s", baseName, ext);

    {
        FILE* in = fopen(argv[1], "r");
        Image* img = loadImage(in); fclose(in);
        Filter f(BLUR);
        f.apply(img);
        FILE* out = fopen(outBlur, "w"); img->save(out); fclose(out);
        delete img;
    }

    {
        FILE* in = fopen(argv[1], "r");
        Image* img = loadImage(in); fclose(in);
        Filter f(LAPLACE);
        f.apply(img);
        FILE* out = fopen(outLaplace, "w"); img->save(out); fclose(out);
        delete img;
    }

    {
        FILE* in = fopen(argv[1], "r");
        Image* img = loadImage(in); fclose(in);
        Filter f(SHARPEN);
        f.apply(img);
        FILE* out = fopen(outSharpen, "w"); img->save(out); fclose(out);
        delete img;
    }

    return 0;
}
