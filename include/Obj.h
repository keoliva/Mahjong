#ifndef OBJ_H
#define OBJ_H
#include <vector>
#include <string>
#include <iostream>
#define COORD_PER_VERTEX 3
#define COORD_PER_FACE 9

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
        std::string name;
        Obj() {};
        Obj(std::string nameObj);
        Obj(char *path, std::string nameObj);
        void loadObj();
        void draw(int x, int y, float rot_x, float r_y);
        virtual ~Obj() { std::cout << "deleting Obj\n"; };
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
        void writeH(std::string fp, Model model);
};

#endif // OBJ_H
