#include "include/Obj.h"
#include "include/tile.h"

#include <fstream>
#include <iostream>
#include <cstring> /* strerror, memcpy */
#include <stdlib.h> /* exit */

#define COORD 3
using namespace std;
void Obj::init()
{
    model = glGenLists(1);
}
Obj::Obj()
{
    init();
    cout << (glGetError()) << endl;
}
Obj::Obj(char *path)
{
    init();
    parseData(path);
}

Model Obj::extractObjData(char *path) {
    ifstream inObj(path, ios::in);
    if (!inObj.good()) {
        cout << strerror(errno);
        exit(1);
    }
    Model model = {0};
    string line, type;
    char *l;
    float x, y, z;
    while (getline(inObj, line)) {
        l = new char[line.size()+1];
        memcpy(l, line.c_str(), line.size()+1);
        type = line.substr(0, 2);
        if (type.compare("v ") == 0) {
            sscanf(l, "v %f %f %f", &x, &y, &z);
            model.vertices.push_back(coord(x, y, z));
        } else if (type.compare("vn") == 0) {
            sscanf(l, "vn %f %f %f", &x, &y, &z);
            model.normals.push_back(coord(x, y, z));
        } else if (type.compare("f ") == 0) {
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
            model.faces.push_back(currFace);
            model.facesNum++;
        } else { continue; }
        delete[] l;
    }
    model.indicesNum = model.facesNum * 3;
    return model;
}

void Obj::parseData(char *path)
{
    Model model = extractObjData(path);
    this->indicesNum = model.indicesNum;
    for (face _face : model.faces) {
        for(int i = 0; i < 3; i++) {
            this->vertices.push_back(_face.vertices[i]);

            this->normals.push_back(_face.normals[i]);
        }
    }

    writeH("include/tile.h", "tile", model);
}
void Obj::writeH(std::string fp, std::string nameObj, Model model)
{
    ofstream outH(fp, ios::out);
    if (!outH.good()) {
        cout << strerror(errno);
        exit(1);
    }
    outH << "// This is a .cpp file for the model: " << nameObj << endl;
    outH << endl;

    outH << "// Faces: " << model.facesNum << endl;
    outH << "// Number of Indices: " << model.indicesNum << endl;

    outH << "namespace " << nameObj << endl;
    outH << "{" << endl;
    outH << "\tconst int indicesNum = " << this->indicesNum << ";" << endl;
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
    cout << "loading" << model;
    glNewList(model, GL_COMPILE);
    {
        glPushMatrix();
        glBegin(GL_TRIANGLES);
            int _size = tile::indicesNum*COORD;
            float vx, vy, vz, nx, ny, nz;
            // every 9 elements represents the xyz-coordinates of the three vertices/normals of a face
            for (int i = 0; i < _size; i+=9) { // COORD*3 = 9
                for (int j = i; j < (i + 9); j+=3) {
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

void Obj::draw(float x, float y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    //glRotatef(90, 0, 1, 0);
    cout << "\ndrawing " << model;
    glCallList(model);
    glPopMatrix();
    glColor3d(1, 1, 1);
}

Obj::~Obj()
{
    //dtor
}
