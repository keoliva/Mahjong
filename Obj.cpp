#include "include/Obj.h"
#include "resources/tile.h"
#include "resources/SOIL.h"
#include <GL/gl.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring> /* strerror, memcpy */
#include <stdlib.h> /* exit */

#define COORD 3
using namespace std;
static int rate = 80, inc = 2;
static bool _dec = true;
void Obj::init()
{
    model = glGenLists(2);
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

void Obj::loadTexture(string path)
{
    unsigned int texID = SOIL_load_OGL_texture
	(
		path.c_str(),
		SOIL_LOAD_RGBA,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);
	if (texID == 0) {
        cout << "SOIL loading error: " << SOIL_last_result() << endl;
	}
    glBindTexture(GL_TEXTURE_2D, texID);
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
    string line, type, material_name, img_filepath;
    float r, g, b;
    char *l;
    while (getline(inMtl, line)) {
        l = new char[line.size()+1];
        memcpy(l, line.c_str(), line.size()+1);
        type = line.substr(0, line.find_first_of(" "));
        if (type.compare("newmtl") == 0) {
            if (mtl.name != "")
                this->materials.push_back(mtl);
            material_name = line.substr(type.size() + 1);
            mtl.name = material_name;
        } else if (type.compare("Kd") == 0) {
            sscanf(l, "Kd %f %f %f", &r, &g, &b);
            mtl.diffuse = rgb(r, g, b);
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
    float x, y, z, u, v;
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
        } else if (type.compare("vt") == 0) {
            sscanf(l, "vt %f %f", &u, &v);
            // in opengl, bottom-left corner is origin, but in .obj file origin is top-left corner
            model.textures.push_back(texcoord(1-u, 1-v));
        } else if (type.compare("vn") == 0) {
            sscanf(l, "vn %f %f %f", &x, &y, &z);
            model.normals.push_back(coord(x, y, z));
        } else if (type.compare("usemtl") == 0) {
            material_name = line.substr(type.size() + 1);
            int i = 0;
            for (material mtl : this->materials) {
                if (material_name.compare(mtl.name) == 0) {
                    mtl_reference = i; break;
                }
                i++;
            }
        } else if (type.compare("f") == 0) {
            face currFace;
            int a, b, c, d, e, f, g, h, i;
            // REQUIRE obj to be triangulated so format of faces is Vertex-Textrue-Normal indices
            sscanf(l, "f %d/%d/%d %d/%d/%d %d/%d/%d", &a, &b, &c, &d, &e, &f, &g, &h, &i);
            // a-i are references starting from 1
            currFace.vertices[0] = model.vertices[a - 1];
            currFace.textures[0] = model.textures[b - 1];
            currFace.normals[0] = model.normals[c - 1];
            currFace.vertices[1] = model.vertices[d - 1];
            currFace.textures[1] = model.textures[e - 1];
            currFace.normals[1] = model.normals[f - 1];
            currFace.vertices[2] = model.vertices[g - 1];
            currFace.textures[2] = model.textures[h - 1];
            currFace.normals[2] = model.normals[i - 1];
            currFace.materialReference = mtl_reference;
            model.faces.push_back(currFace);
            model.facesNum++;
        } else { continue; }
        delete[] l;
    }
    inObj.close();
    return model;
}

void Obj::parseData(string path)
{
    Model model = extractObjData(path);
    this->facesNum = model.facesNum;
    material mtl = material();
    for (face _face : model.faces) {
        mtl = this->materials[_face.materialReference];
        this->diffuses.push_back(mtl.diffuse);
        for(int i = 0; i < VERTICES_PER_FACE; i++) {
            this->vertices.push_back(_face.vertices[i]);
            this->textures.push_back(_face.textures[i]);
            this->normals.push_back(_face.normals[i]);
        }
    }
    string filename = "resources/" + name + ".h";
    writeH(filename);
}
void Obj::writeH(std::string fp)
{
    ofstream outH(fp, ios::out);
    if (!outH.good()) {
        cout << strerror(errno);
        exit(1);
    }
    outH << "// This is a .cpp file for the model: " << name << endl;
    outH << endl;

    outH << "// Faces: " << this->facesNum << endl;

    outH << "namespace " << name << endl;
    outH << "{" << endl;
    outH << "\tconst int facesNum = " << this->facesNum << ";" << endl;

    outH << "\tconst float diffuses[" << this->facesNum << "][3] = {" << endl;
    for (rgb d : this->diffuses)
        outH << "\t\t{" << d.r << ", " << d.g << ", " << d.b << "}," << endl;
    outH << "\t};" << endl;

    outH << "\tconst float textures[" << this->facesNum << "][" << VERTICES_PER_FACE << "][2] = {" << endl;
    int tex_seen = 0;
    texcoord t;
    for (int face = 0; face < this->facesNum; face++) {
        outH << "\t\t{" << endl;
        outH << "\t\t\t";
        for (int i = 0; i < VERTICES_PER_FACE; i++) { // this->vertices.size() == this->textures.size()
            t = this->textures[tex_seen++];
            outH << "{" << t.u << ", " << t.v << "}, ";
        }
        outH << endl;
        outH << "\t\t}," << endl;
    }
    outH << "\t};" << endl;

    outH << "\tconst float vertices[" << this->facesNum << "][" << VERTICES_PER_FACE << "][3] = {" << endl;
    int vert_seen = 0;
    coord v;
    for (int face = 0; face < this->facesNum; face++) {
        outH << "\t\t{" << endl;
        for (int i = 0; i < VERTICES_PER_FACE; i++) {
            v = this->vertices[vert_seen++];
            outH << "\t\t\t{" << v.x << ", " << v.y << ", " << v.z << "}," << endl;
        }
        outH << "\t\t}," << endl;
    }
    outH << "\t};" << endl;

    outH << "\tconst float normals[" << this->facesNum << "][" << VERTICES_PER_FACE << "][3] = {" << endl;
    int norm_seen = 0;
    coord n;
    for (int face = 0; face < this->facesNum; face++) {
        outH << "\t\t{" << endl;
        for (int i = 0; i < VERTICES_PER_FACE; i++) {// this->normals.size() == this->textures.size()
            n = this->normals[norm_seen++];
            outH << "\t\t\t{" << n.x << ", " << n.y << ", " << n.z << "}," << endl;
        }
        outH << "\t\t}," << endl;
    }
    outH << "\t};" << endl;

    outH << "}" << endl;
    outH.close();
}
void loadBlinkingObj(int i, float opacity);
void Obj::loadObj()
{
    int i = model + 1, total = i + rate;
    float opacity = 1.0f, dec_amount = opacity/(float)rate;
    loadBlinkingObj(model, opacity);
    for (; i <= total; i++) {
        loadBlinkingObj(i, opacity);
        opacity -= dec_amount;
    }
}

void loadBlinkingObj(int i, float opacity) {
// simulates a blinking object
    glNewList(i, GL_COMPILE);
    {
        glPushMatrix();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        glBegin(GL_TRIANGLES);
            int faces = tile::facesNum;
            float vert[3], norm[3], uv[2], rgb[3];
            // every 9 elements represents the xyz-coordinates of the three vertices/normals of a face
            for (int i = 0; i < faces; i++) { // COORD*3 = 9
                memcpy(rgb, tile::diffuses[i], sizeof(rgb));
                glColor4f(rgb[0], rgb[1], rgb[2], opacity);
                for (int j = 0; j < VERTICES_PER_FACE; j++) {
                    memcpy(vert, tile::vertices[i][j], sizeof(vert));
                    memcpy(norm, tile::normals[i][j], sizeof(norm));
                    memcpy(uv, tile::textures[i][j], sizeof(uv));
                    glTexCoord2f(uv[0], uv[1]); // switched since mirror version showed instead
                    glNormal3f(norm[0], norm[1], norm[2]);
                    glVertex3f(vert[0], vert[1], vert[2]);
                }
            }
        glEnd();
        glDisable(GL_BLEND);
        glPopMatrix();
    }
    glEndList();
}

void Obj::draw(float x, float y, float z,
               float rot_x, float rot_y, float rot_z,
               string texture_filename,
               bool blinking)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rot_x, 1, 0, 0);
    glRotatef(rot_y, 0, 1, 0);
    glRotatef(rot_z, 0, 0, 1);

    if (texture_filename != "") {
        glEnable(GL_TEXTURE_2D);
        loadTexture("resources/images/"+texture_filename+".png");
    }
    if (!blinking) {
        glCallList(model); // model = 1
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
        glColor3d(1, 1, 1);
        return;
    }
    glCallList(inc);
    // since glGenLists(n) will return 1, 2 and onwards represents the
    // code (or list) with varying opacities
    while (!((_dec && inc > 2) || (!_dec && inc < 82))) {
        if (_dec && inc <= 2) {
            _dec = false;
        }
        else if (!_dec && inc >= 82) {
            _dec = true;
        }
    }
    if (_dec) {
        inc -= 1;
    } else {
        inc += 1;
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glColor3d(1, 1, 1);
}
