#ifndef OBJ_H
#define OBJ_H
#include <vector>
#include <string>
#include <iostream>
#define VERTICES_PER_FACE 3
#define COORD_PER_TEX 3
#define COORD_PER_VERTEX 3


typedef struct coord {
    float x, y, z;
    coord() {};
    coord(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {};
} coord;
typedef struct texcoord {
    float u, v;
    texcoord() {};
    texcoord(float a, float b) : u(a), v(b) {};
} texcoord;
typedef struct rgb {
    float r, g, b;
    rgb() {};
    rgb(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {};
} rgb;
typedef struct material {
    std::string name;
    rgb diffuse;
    material() {};
} material;
// in this project, obj files are being exported where the faces are triangulated
// so there are only three vertices per face
typedef struct face {
    coord vertices[3];
    texcoord textures[3];
    coord normals[3];
    int materialReference;
} face;
typedef struct Model {
    int facesNum;
    std::vector<coord> vertices, normals;
    std::vector<texcoord> textures;
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
        void draw(float x, float y, float z,
                  float rot_x, float r_y, float r_z=0.0f,
                  std::string texture_filename="",
                  bool blinking=false);
        virtual ~Obj() { std::cout << "deleting Obj\n"; };
    private:
        int model;
        std::vector<coord> vertices, normals;
        std::vector<texcoord> textures;
        std::vector<rgb> diffuses; // intrinsic color of object
        std::vector<material> materials;
        int facesNum;

        void init();
        void loadTexture(std::string path);
        void extractMaterialsData(std::string path);
        Model extractObjData(std::string path);
        void parseData(std::string path);
        void writeH(std::string fp);
};

#endif // OBJ_H
