#include "include/Obj.h"
#include "resources/tile.h"
#include <GL/gl.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring> /* strerror, memcpy */
#include <stdlib.h> /* exit */

#define COORD 3
using namespace std;
void Obj::init()
{
    model = glGenLists(1);
}
Obj::Obj(string nameObj)
{
    name = nameObj;
    init();
}
Obj::Obj(char *path, string nameObj)
{
    name = nameObj;
    init();
    parseData(path);
}

void Obj::extractMaterialsData(string path)
{
    string mtl_path = "resources/" + path;
    ifstream inMtl(mtl_path, ios::in);
    if (!inMtl.good()) {
        cout << strerror(errno);
        exit(1);
    }
    material mtl = material();
    string line, type, material_name;
    float r, g, b;
    char *l;
    while (getline(inMtl, line)) {
        l = new char[line.size()+1];
        memcpy(l, line.c_str(), line.size()+1);
        type = line.substr(0, line.find_first_of(" "));
        if (type.compare("newmtl") == 0) {
            if (mtl.name != "") this->materials.push_back(mtl);
            material_name = line.substr(type.size() + 1);
            mtl.name = material_name;
        } else if (type.compare("Kd") == 0) {
            sscanf(l, "Kd %f %f %f", &r, &g, &b);
            mtl.diffuse = rgb(r, g, b);
        } else if (type.compare("Ks")) {
            sscanf(l, "Ks %f %f %f", &r, &g, &b);
            mtl.specular = rgb(r, g, b);
        } else { continue; }
        delete[] l;
    }
    inMtl.close();
    this->materials.push_back(mtl);
}

Model Obj::extractObjData(string path) {
    ifstream inObj(path, ios::in);
    if (!inObj.good()) {
        cout << strerror(errno);
        exit(1);
    }
    Model model = {0};
    string line, type, mtl_filename, material_name;
    int mtl_reference;
    char *l;
    float x, y, z;
    while (getline(inObj, line)) {
        l = new char[line.size()+1];
        memcpy(l, line.c_str(), line.size()+1);
        type = line.substr(0, line.find_first_of(" "));
        if (type.compare("mtllib") == 0) {
            mtl_filename = line.substr(type.size() + 1);
            extractMaterialsData(mtl_filename.c_str());
        } else if (type.compare("v") == 0) {
            sscanf(l, "v %f %f %f", &x, &y, &z);
            model.vertices.push_back(coord(x, y, z));
        } else if (type.compare("vn") == 0) {
            sscanf(l, "vn %f %f %f", &x, &y, &z);
            model.normals.push_back(coord(x, y, z));
        } else if (type.compare("usemtl") == 0) {
            material_name = line.substr(type.size() + 1);
            int i = 0;
            for (material mtl : this->materials) {
                if (material_name.compare(mtl.name) == 0) {
                    mtl_reference = i;
                    break;
                }
                i++;
            }
        } else if (type.compare("f") == 0) {
            face currFace;
            int a, b, c, d, e, f;
            // REQUIRE obj to be triangulated so format of faces is
            // Vertex-Normal Indices Without Texture Coordinate Indices
            sscanf(l, "f %d//%d %d//%d %d//%d", &a, &b, &c, &d, &e, &f);
            // v0-v2, n0-n2 are indices references starting from 1
            currFace.vertices[0] = model.vertices[a - 1];
            currFace.normals[0] = model.normals[b - 1];
            currFace.vertices[1] = model.vertices[c - 1];
            currFace.normals[1] = model.normals[d - 1];
            currFace.vertices[2] = model.vertices[e - 1];
            currFace.normals[2] = model.normals[f - 1];
            currFace.materialReference = mtl_reference;
            model.faces.push_back(currFace);
            model.facesNum++;
        } else { continue; }
        delete[] l;
    }
    inObj.close();
    model.indicesNum = model.facesNum * 3;
    return model;
}

void Obj::parseData(string path)
{
    Model model = extractObjData(path);
    this->indicesNum = model.indicesNum;
    material mtl = material();
    for (face _face : model.faces) {
        mtl = this->materials[_face.materialReference];
        this->diffuses.push_back(mtl.diffuse);
        for(int i = 0; i < 3; i++) {
            this->vertices.push_back(_face.vertices[i]);
            this->normals.push_back(_face.normals[i]);
        }
    }
    stringstream ss;
    string filename;
    ss << "include/" << name << ".h";
    ss >> filename;
    writeH(filename, model);
}
void Obj::writeH(std::string fp, Model model)
{
    ofstream outH(fp, ios::out);
    if (!outH.good()) {
        cout << strerror(errno);
        exit(1);
    }
    outH << "// This is a .cpp file for the model: " << name << endl;
    outH << endl;

    outH << "// Faces: " << model.facesNum << endl;
    outH << "// Number of Indices: " << model.indicesNum << endl;

    outH << "namespace " << name << endl;
    outH << "{" << endl;
    outH << "\tconst int indicesNum = " << this->indicesNum << ";" << endl;

    outH << "\tconst float diffuses[" << model.facesNum << "][3] = {" << endl;
    for (rgb d : this->diffuses)
        outH << "\t\t{" << d.r << ", " << d.g << ", " << d.b << "}," << endl;
    outH << "\t};" << endl;

    outH << "\tconst float vertices[" << this->vertices.size()*3 << "] = {" << endl;
    for (coord v : this->vertices)
        outH << "\t\t" << v.x << ", " << v.y << ", " << v.z << "," << endl;
    outH << "\t};" << endl;

    outH << "\tconst float normals[" << this->normals.size()*3 << "] = {" << endl;
     for (coord v : this->normals)
        outH << "\t\t" << v.x << ", " << v.y << ", " << v.z << "," << endl;
    outH << "\t};" << endl;

    outH << "}" << endl;
    outH.close();
}

void Obj::loadObj()
{
    glNewList(model, GL_COMPILE);
    {
        glPushMatrix();
        glBegin(GL_TRIANGLES);
            int _size = tile::indicesNum*COORD;
            float vx, vy, vz, nx, ny, nz;
            float rgb[3];
            // every 9 elements represents the xyz-coordinates of the three vertices/normals of a face
            for (int i = 0; i < _size; i+=COORD_PER_FACE) { // COORD*3 = 9
                memcpy(rgb, tile::diffuses[i/COORD_PER_FACE], sizeof(rgb));
                glColor3f(rgb[0], rgb[1], rgb[2]);
                for (int j = i; j < (i + COORD_PER_FACE); j+=COORD_PER_VERTEX) {
                    vx = tile::vertices[j]; vy = tile::vertices[j+1]; vz = tile::vertices[j+2];
                    nx = tile::normals[j]; ny = tile::normals[j+1]; nz = tile::normals[j+2];

                    glNormal3f(nx, ny, nz);
                    glVertex3f(vx, vy, vz);
                }
            }
        glEnd();
    }
    glPopMatrix();
    glEndList();
}

void Obj::draw(float x, float y, float z, float rot_x, float rot_y, float rot_z)
{
    glPushMatrix();
    glTranslatef(x, y, z);

    glRotatef(rot_x, 1, 0, 0);
    glRotatef(rot_y, 0, 1, 0);
    glRotatef(rot_z, 0, 0, 1);

    glCallList(model);
    glPopMatrix();
    glColor3d(1, 1, 1);
}
