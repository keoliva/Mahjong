#ifndef OBJ_H
#define OBJ_H
#include <vector>
#include <string>
#include <GL/gl.h>
#define MAX_LEN_MATERIAL_NAME 200

typedef struct coord {
    float x, y, z;
    coord() {};
    coord(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
} coord;

typedef struct rgb {
    float r, g, b;
    rgb() {};
    rgb(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {};
} rgb;

typedef struct material {
    std::string name;
    float alpha, ns, ni;
    rgb diffuse, specular;
    int illum;
    int texture;
    material() {};
} material;

// in this project, obj files are being exported where texture is not included
// and the faces are triangulated
typedef struct face {
    coord vertices[3];
    coord normals[3];
    int materialReference;
} face;

typedef struct Model {
    int facesNum, indicesNum;
    std::vector<coord> vertices, normals;
    std::vector<face> faces;
} Model;

class Obj
{
    public:
        Obj();
        Obj(char *path);
        void loadObj();
        void draw(float x, float y);
        virtual ~Obj();
    private:
        int model;
        std::vector<coord> vertices, normals;
        std::vector<rgb> diffuses;
        std::vector<material> materials;
        int indicesNum;

        void init();
        void extractMaterialsData(std::string path);
        Model extractObjData(std::string path);
        void parseData(std::string path);
        void writeH(std::string fp, std::string nameObj, Model model);
};

#endif // OBJ_H
