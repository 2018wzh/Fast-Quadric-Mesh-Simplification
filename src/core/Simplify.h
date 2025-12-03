/////////////////////////////////////////////
//
// Mesh Simplification Tutorial
//
// (C) by Sven Forstmann in 2014
//
// License : MIT
// http://opensource.org/licenses/MIT
//
// https://github.com/sp4cerat/Fast-Quadric-Mesh-Simplification
//
// 5/2016: Chris Rorden created minimal version for OSX/Linux/Windows compile

// #include <iostream>
// #include <stddef.h>
// #include <functional>
// #include <sys/stat.h>
// #include <stdbool.h>
#include <ctype.h>
#include <string.h>
// #include <float.h>
#include <float.h> //FLT_EPSILON, DBL_EPSILON
#include <map>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#define loopi(start_l, end_l) for (int i = start_l; i < end_l; ++i)
#define loopi(start_l, end_l) for (int i = start_l; i < end_l; ++i)
#define loopj(start_l, end_l) for (int j = start_l; j < end_l; ++j)
#define loopk(start_l, end_l) for (int k = start_l; k < end_l; ++k)

#define MAX_LINE_LENGTH 16384

struct vector3 {
    double x, y, z;
};

struct vec3f {
    double x, y, z;

    inline vec3f(void) {}

    // inline vec3f operator =( vector3 a )
    //  { vec3f b ; b.x = a.x; b.y = a.y; b.z = a.z; return b;}

    inline vec3f(vector3 a) {
        x = a.x;
        y = a.y;
        z = a.z;
    }

    inline vec3f(const double X, const double Y, const double Z) {
        x = X;
        y = Y;
        z = Z;
    }

    inline vec3f operator+(const vec3f &a) const { return vec3f(x + a.x, y + a.y, z + a.z); }

    inline vec3f operator+=(const vec3f &a) const { return vec3f(x + a.x, y + a.y, z + a.z); }

    inline vec3f operator*(const double a) const { return vec3f(x * a, y * a, z * a); }

    inline vec3f operator*(const vec3f a) const { return vec3f(x * a.x, y * a.y, z * a.z); }

    inline vec3f v3() const { return vec3f(x, y, z); }

    inline vec3f operator=(const vector3 a) {
        x = a.x;
        y = a.y;
        z = a.z;
        return *this;
    }

    inline vec3f operator=(const vec3f a) {
        x = a.x;
        y = a.y;
        z = a.z;
        return *this;
    }

    inline vec3f operator/(const vec3f a) const { return vec3f(x / a.x, y / a.y, z / a.z); }

    inline vec3f operator-(const vec3f &a) const { return vec3f(x - a.x, y - a.y, z - a.z); }

    inline vec3f operator/(const double a) const { return vec3f(x / a, y / a, z / a); }

    inline double dot(const vec3f &a) const { return a.x * x + a.y * y + a.z * z; }

    inline vec3f cross(const vec3f &a, const vec3f &b) {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
        return *this;
    }

    inline double angle(const vec3f &v) {
        vec3f a = v, b = *this;
        double dot = v.x * x + v.y * y + v.z * z;
        double len = a.length() * b.length();
        if (len == 0)
            len = 0.00001f;
        double input = dot / len;
        if (input < -1)
            input = -1;
        if (input > 1)
            input = 1;
        return (double)acos(input);
    }

    inline double angle2(const vec3f &v, const vec3f &w) {
        vec3f a = v, b = *this;
        double dot = a.x * b.x + a.y * b.y + a.z * b.z;
        double len = a.length() * b.length();
        if (len == 0)
            len = 1;

        vec3f plane;
        plane.cross(b, w);

        if (plane.x * a.x + plane.y * a.y + plane.z * a.z > 0)
            return (double)-acos(dot / len);

        return (double)acos(dot / len);
    }

    inline vec3f rot_x(double a) {
        double yy = cos(a) * y + sin(a) * z;
        double zz = cos(a) * z - sin(a) * y;
        y         = yy;
        z         = zz;
        return *this;
    }
    inline vec3f rot_y(double a) {
        double xx = cos(-a) * x + sin(-a) * z;
        double zz = cos(-a) * z - sin(-a) * x;
        x         = xx;
        z         = zz;
        return *this;
    }
    inline void clamp(double min, double max) {
        if (x < min)
            x = min;
        if (y < min)
            y = min;
        if (z < min)
            z = min;
        if (x > max)
            x = max;
        if (y > max)
            y = max;
        if (z > max)
            z = max;
    }
    inline vec3f rot_z(double a) {
        double yy = cos(a) * y + sin(a) * x;
        double xx = cos(a) * x - sin(a) * y;
        y         = yy;
        x         = xx;
        return *this;
    }
    inline vec3f invert() {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }
    inline vec3f frac() {
        return vec3f(x - double(int(x)), y - double(int(y)), z - double(int(z)));
    }

    inline vec3f integer() { return vec3f(double(int(x)), double(int(y)), double(int(z))); }

    inline double length() const { return (double)sqrt(x * x + y * y + z * z); }

    inline vec3f normalize(double desired_length = 1) {
        double square = sqrt(x * x + y * y + z * z);
        /*
        if (square <= 0.00001f )
        {
                x=1;y=0;z=0;
                return *this;
        }*/
        // double len = desired_length / square;
        x /= square;
        y /= square;
        z /= square;

        return *this;
    }
    static vec3f normalize(vec3f a);

    static void random_init();
    static double random_double();
    static vec3f random();

    static int random_number;

    double random_double_01(double a) {
        double rnf = a * 14.434252 + a * 364.2343 + a * 4213.45352 + a * 2341.43255 +
                     a * 254341.43535 + a * 223454341.3523534245 + 23453.423412;
        int rni = ((int)rnf) % 100000;
        return double(rni) / (100000.0f - 1.0f);
    }

    vec3f random01_fxyz() {
        x = (double)random_double_01(x);
        y = (double)random_double_01(y);
        z = (double)random_double_01(z);
        return *this;
    }
};

vec3f barycentric(const vec3f &p, const vec3f &a, const vec3f &b, const vec3f &c) {
    vec3f v0     = b - a;
    vec3f v1     = c - a;
    vec3f v2     = p - a;
    double d00   = v0.dot(v0);
    double d01   = v0.dot(v1);
    double d11   = v1.dot(v1);
    double d20   = v2.dot(v0);
    double d21   = v2.dot(v1);
    double denom = d00 * d11 - d01 * d01;
    double v     = (d11 * d20 - d01 * d21) / denom;
    double w     = (d00 * d21 - d01 * d20) / denom;
    double u     = 1.0 - v - w;
    return vec3f(u, v, w);
}

vec3f interpolate(const vec3f &p, const vec3f &a, const vec3f &b, const vec3f &c,
                  const vec3f attrs[3]) {
    vec3f bary = barycentric(p, a, b, c);
    vec3f out  = vec3f(0, 0, 0);
    out        = out + attrs[0] * bary.x;
    out        = out + attrs[1] * bary.y;
    out        = out + attrs[2] * bary.z;
    return out;
}

double min(double v1, double v2) { return fmin(v1, v2); }

class SymetricMatrix {

  public:
    // Constructor

    SymetricMatrix(double c = 0) { loopi(0, 10) m[i] = c; }

    SymetricMatrix(double m11, double m12, double m13, double m14, double m22, double m23,
                   double m24, double m33, double m34, double m44) {
        m[0] = m11;
        m[1] = m12;
        m[2] = m13;
        m[3] = m14;
        m[4] = m22;
        m[5] = m23;
        m[6] = m24;
        m[7] = m33;
        m[8] = m34;
        m[9] = m44;
    }

    // Make plane

    SymetricMatrix(double a, double b, double c, double d) {
        m[0] = a * a;
        m[1] = a * b;
        m[2] = a * c;
        m[3] = a * d;
        m[4] = b * b;
        m[5] = b * c;
        m[6] = b * d;
        m[7] = c * c;
        m[8] = c * d;
        m[9] = d * d;
    }

    double operator[](int c) const { return m[c]; }

    // Determinant

    double det(int a11, int a12, int a13, int a21, int a22, int a23, int a31, int a32, int a33) {
        double det = m[a11] * m[a22] * m[a33] + m[a13] * m[a21] * m[a32] +
                     m[a12] * m[a23] * m[a31] - m[a13] * m[a22] * m[a31] -
                     m[a11] * m[a23] * m[a32] - m[a12] * m[a21] * m[a33];
        return det;
    }

    const SymetricMatrix operator+(const SymetricMatrix &n) const {
        return SymetricMatrix(m[0] + n[0], m[1] + n[1], m[2] + n[2], m[3] + n[3], m[4] + n[4],
                              m[5] + n[5], m[6] + n[6], m[7] + n[7], m[8] + n[8], m[9] + n[9]);
    }

    SymetricMatrix &operator+=(const SymetricMatrix &n) {
        m[0] += n[0];
        m[1] += n[1];
        m[2] += n[2];
        m[3] += n[3];
        m[4] += n[4];
        m[5] += n[5];
        m[6] += n[6];
        m[7] += n[7];
        m[8] += n[8];
        m[9] += n[9];
        return *this;
    }

    double m[10];
};
///////////////////////////////////////////

namespace Simplify {
// Global Variables & Strctures
enum Attributes { NONE, NORMAL = 2, TEXCOORD = 4, COLOR = 8 };
struct Triangle {
    int v[3];
    double err[4];
    int deleted, dirty, attr;
    vec3f n;
    vec3f uvs[3];
    int material;
};
struct Vertex {
    vec3f p;
    int tstart, tcount;
    SymetricMatrix q;
    int border;
};
struct Ref {
    int tid, tvertex;
};
std::vector<Triangle> triangles;
std::vector<Vertex> vertices;
std::vector<Ref> refs;
std::string mtllib;
std::vector<std::string> materials;

// Helper functions

double vertex_error(SymetricMatrix q, double x, double y, double z);
double calculate_error(int id_v1, int id_v2, vec3f &p_result);
bool flipped(vec3f p, int i0, int i1, Vertex &v0, Vertex &v1, std::vector<int> &deleted);
void update_uvs(int i0, const Vertex &v, const vec3f &p, std::vector<int> &deleted);
void update_triangles(int i0, Vertex &v, std::vector<int> &deleted, int &deleted_triangles);
void update_mesh(int iteration);
void compact_mesh();
//
// Main simplification function
//
// target_count  : target nr. of triangles
// agressiveness : sharpness to increase the threshold.
//                 5..8 are good numbers
//                 more iterations yield higher quality
//

void simplify_mesh(int target_count, double agressiveness = 7) {
    // init
    loopi(0, triangles.size()) { triangles[i].deleted = 0; }

    // main iteration loop
    int deleted_triangles = 0;
    std::vector<int> deleted0, deleted1;
    int triangle_count = triangles.size();
    // int iteration = 0;
    // loop(iteration,0,100)
    for (int iteration = 0; iteration < 100; iteration++) {
        if (triangle_count - deleted_triangles <= target_count)
            break;

        // update mesh once in a while
        if (iteration % 5 == 0) {
            update_mesh(iteration);
        }

        // clear dirty flag
        loopi(0, triangles.size()) triangles[i].dirty = 0;

        //
        // All triangles with edges below the threshold will be removed
        //
        // The following numbers works well for most models.
        // If it does not, try to adjust the 3 parameters
        //
        double threshold = 0.000000001 * pow(double(iteration + 3), agressiveness);

        // target number of triangles reached ? Then break
        printf("iteration %d - triangles %d threshold %g\n", iteration,
               triangle_count - deleted_triangles, threshold);

        // remove vertices & mark deleted triangles
        loopi(0, triangles.size()) {
            Triangle &t = triangles[i];
            if (t.err[3] > threshold)
                continue;
            if (t.deleted)
                continue;
            if (t.dirty)
                continue;

            loopj(0, 3) if (t.err[j] < threshold) {

                int i0     = t.v[j];
                Vertex &v0 = vertices[i0];
                int i1     = t.v[(j + 1) % 3];
                Vertex &v1 = vertices[i1];
                // Border check
                if (v0.border != v1.border)
                    continue;

                // Compute vertex to collapse to
                vec3f p;
                calculate_error(i0, i1, p);
                deleted0.resize(v0.tcount); // normals temporarily
                deleted1.resize(v1.tcount); // normals temporarily
                // don't remove if flipped
                if (flipped(p, i0, i1, v0, v1, deleted0))
                    continue;

                if (flipped(p, i1, i0, v1, v0, deleted1))
                    continue;

                if ((t.attr & TEXCOORD) == TEXCOORD) {
                    update_uvs(i0, v0, p, deleted0);
                    update_uvs(i0, v1, p, deleted1);
                }

                // not flipped, so remove edge
                v0.p       = p;
                v0.q       = v1.q + v0.q;
                int tstart = refs.size();

                update_triangles(i0, v0, deleted0, deleted_triangles);
                update_triangles(i0, v1, deleted1, deleted_triangles);

                int tcount = refs.size() - tstart;

                if (tcount <= v0.tcount) {
                    // save ram
                    if (tcount)
                        memcpy(&refs[v0.tstart], &refs[tstart], tcount * sizeof(Ref));
                } else
                    // append
                    v0.tstart = tstart;

                v0.tcount = tcount;
                break;
            }
            // done?
            if (triangle_count - deleted_triangles <= target_count)
                break;
        }
    }
    // clean up mesh
    compact_mesh();
} // simplify_mesh()

void simplify_mesh_lossless(int target_count) {
    // init
    loopi(0, triangles.size()) triangles[i].deleted = 0;

    // main iteration loop
    int deleted_triangles      = 0;
    int last_deleted_triangles = 0;
    std::vector<int> deleted0, deleted1;
    int triangle_count = triangles.size();
    // int iteration = 0;
    // loop(iteration,0,100)
    for (int iteration = 0; iteration < 9999; iteration++) {
        if (triangle_count - deleted_triangles <= target_count)
            break;
        // update mesh constantly
        update_mesh(iteration);
        // clear dirty flag
        loopi(0, triangles.size()) triangles[i].dirty = 0;
        //
        // All triangles with edges below the threshold will be removed
        //
        // The following numbers works well for most models.
        // If it does not, try to adjust the 3 parameters
        //
        double threshold = DBL_EPSILON; // 1.0E-3 EPS;
        printf("iteration %d - triangles %d threshold %g\n", iteration,
               triangle_count - deleted_triangles, threshold);

        // remove vertices & mark deleted triangles
        loopi(0, triangles.size()) {
            Triangle &t = triangles[i];
            if (t.err[3] > threshold)
                continue;
            if (t.deleted)
                continue;
            if (t.dirty)
                continue;

            loopj(0, 3) if (t.err[j] < threshold) {
                int i0     = t.v[j];
                Vertex &v0 = vertices[i0];
                int i1     = t.v[(j + 1) % 3];
                Vertex &v1 = vertices[i1];

                // Border check
                if (v0.border != v1.border)
                    continue;

                // Compute vertex to collapse to
                vec3f p;
                calculate_error(i0, i1, p);

                deleted0.resize(v0.tcount); // normals temporarily
                deleted1.resize(v1.tcount); // normals temporarily

                // don't remove if flipped
                if (flipped(p, i0, i1, v0, v1, deleted0))
                    continue;
                if (flipped(p, i1, i0, v1, v0, deleted1))
                    continue;

                if ((t.attr & TEXCOORD) == TEXCOORD) {
                    update_uvs(i0, v0, p, deleted0);
                    update_uvs(i0, v1, p, deleted1);
                }

                // not flipped, so remove edge
                v0.p       = p;
                v0.q       = v1.q + v0.q;
                int tstart = refs.size();

                update_triangles(i0, v0, deleted0, deleted_triangles);
                update_triangles(i0, v1, deleted1, deleted_triangles);

                int tcount = refs.size() - tstart;

                if (tcount <= v0.tcount) {
                    // save ram
                    if (tcount)
                        memcpy(&refs[v0.tstart], &refs[tstart], tcount * sizeof(Ref));
                } else
                    // append
                    v0.tstart = tstart;

                v0.tcount = tcount;
                break;
            }
            if (triangle_count - deleted_triangles <= target_count)
                break;
        }
        if (deleted_triangles == last_deleted_triangles)
            break;
        last_deleted_triangles = deleted_triangles;
    } // for each iteration
    // clean up mesh
    compact_mesh();
} // simplify_mesh_lossless()

// Check if a triangle flips when this edge is removed

bool flipped(vec3f p, int i0, int i1, Vertex &v0, Vertex &v1, std::vector<int> &deleted) {

    loopk(0, v0.tcount) {
        Triangle &t = triangles[refs[v0.tstart + k].tid];
        if (t.deleted)
            continue;

        int s   = refs[v0.tstart + k].tvertex;
        int id1 = t.v[(s + 1) % 3];
        int id2 = t.v[(s + 2) % 3];

        if (id1 == i1 || id2 == i1) // delete ?
        {

            deleted[k] = 1;
            continue;
        }
        vec3f d1 = vertices[id1].p - p;
        d1.normalize();
        vec3f d2 = vertices[id2].p - p;
        d2.normalize();
        if (fabs(d1.dot(d2)) > 0.999)
            return true;
        vec3f n;
        n.cross(d1, d2);
        n.normalize();
        deleted[k] = 0;
        if (n.dot(t.n) < 0.2)
            return true;
    }
    return false;
}

// update_uvs

void update_uvs(int i0, const Vertex &v, const vec3f &p, std::vector<int> &deleted) {
    loopk(0, v.tcount) {
        Ref &r      = refs[v.tstart + k];
        Triangle &t = triangles[r.tid];
        if (t.deleted)
            continue;
        if (deleted[k])
            continue;
        vec3f p1         = vertices[t.v[0]].p;
        vec3f p2         = vertices[t.v[1]].p;
        vec3f p3         = vertices[t.v[2]].p;
        t.uvs[r.tvertex] = interpolate(p, p1, p2, p3, t.uvs);
    }
}

// Update triangle connections and edge error after a edge is collapsed

void update_triangles(int i0, Vertex &v, std::vector<int> &deleted, int &deleted_triangles) {
    vec3f p;
    loopk(0, v.tcount) {
        Ref &r      = refs[v.tstart + k];
        Triangle &t = triangles[r.tid];
        if (t.deleted)
            continue;
        if (deleted[k]) {
            t.deleted = 1;
            deleted_triangles++;
            continue;
        }
        t.v[r.tvertex] = i0;
        t.dirty        = 1;
        t.err[0]       = calculate_error(t.v[0], t.v[1], p);
        t.err[1]       = calculate_error(t.v[1], t.v[2], p);
        t.err[2]       = calculate_error(t.v[2], t.v[0], p);
        t.err[3]       = min(t.err[0], min(t.err[1], t.err[2]));
        refs.push_back(r);
    }
}

// compact triangles, compute edge error and build reference list

void update_mesh(int iteration) {
    if (iteration > 0) // compact triangles
    {
        int dst = 0;
        loopi(0, triangles.size()) if (!triangles[i].deleted) { triangles[dst++] = triangles[i]; }
        triangles.resize(dst);
    }
    //

    // Init Reference ID list
    loopi(0, vertices.size()) {
        vertices[i].tstart = 0;
        vertices[i].tcount = 0;
    }
    loopi(0, triangles.size()) {
        Triangle &t = triangles[i];
        loopj(0, 3) vertices[t.v[j]].tcount++;
    }
    int tstart = 0;
    loopi(0, vertices.size()) {
        Vertex &v = vertices[i];
        v.tstart  = tstart;
        tstart += v.tcount;
        v.tcount = 0;
    }

    // Write References
    refs.resize(triangles.size() * 3);
    loopi(0, triangles.size()) {
        Triangle &t = triangles[i];
        loopj(0, 3) {
            Vertex &v                         = vertices[t.v[j]];
            refs[v.tstart + v.tcount].tid     = i;
            refs[v.tstart + v.tcount].tvertex = j;
            v.tcount++;
        }
    }

    // Init Quadrics by Plane & Edge Errors
    //
    // required at the beginning ( iteration == 0 )
    // recomputing during the simplification is not required,
    // but mostly improves the result for closed meshes
    //
    if (iteration == 0) {
        // Identify boundary : vertices[].border=0,1

        std::vector<int> vcount, vids;

        loopi(0, vertices.size()) vertices[i].border = 0;

        loopi(0, vertices.size()) {
            Vertex &v = vertices[i];
            vcount.clear();
            vids.clear();
            loopj(0, v.tcount) {
                int k       = refs[v.tstart + j].tid;
                Triangle &t = triangles[k];
                loopk(0, 3) {
                    int ofs = 0, id = t.v[k];
                    while (ofs < vcount.size()) {
                        if (vids[ofs] == id)
                            break;
                        ofs++;
                    }
                    if (ofs == vcount.size()) {
                        vcount.push_back(1);
                        vids.push_back(id);
                    } else
                        vcount[ofs]++;
                }
            }
            loopj(0, vcount.size()) if (vcount[j] == 1) vertices[vids[j]].border = 1;
        }
        // initialize errors
        loopi(0, vertices.size()) vertices[i].q = SymetricMatrix(0.0);

        loopi(0, triangles.size()) {
            Triangle &t = triangles[i];
            vec3f n, p[3];
            loopj(0, 3) p[j] = vertices[t.v[j]].p;
            n.cross(p[1] - p[0], p[2] - p[0]);
            n.normalize();
            t.n = n;
            loopj(0, 3) vertices[t.v[j]].q =
                vertices[t.v[j]].q + SymetricMatrix(n.x, n.y, n.z, -n.dot(p[0]));
        }
        loopi(0, triangles.size()) {
            // Calc Edge Error
            Triangle &t = triangles[i];
            vec3f p;
            loopj(0, 3) t.err[j] = calculate_error(t.v[j], t.v[(j + 1) % 3], p);
            t.err[3]             = min(t.err[0], min(t.err[1], t.err[2]));
        }
    }
}

// Finally compact mesh before exiting

void compact_mesh() {
    int dst = 0;
    loopi(0, vertices.size()) { vertices[i].tcount = 0; }
    loopi(0, triangles.size()) if (!triangles[i].deleted) {
        Triangle &t                         = triangles[i];
        triangles[dst++]                    = t;
        loopj(0, 3) vertices[t.v[j]].tcount = 1;
    }
    triangles.resize(dst);
    dst = 0;
    loopi(0, vertices.size()) if (vertices[i].tcount) {
        vertices[i].tstart = dst;
        vertices[dst].p    = vertices[i].p;
        dst++;
    }
    loopi(0, triangles.size()) {
        Triangle &t        = triangles[i];
        loopj(0, 3) t.v[j] = vertices[t.v[j]].tstart;
    }
    vertices.resize(dst);
}

// Error between vertex and Quadric

double vertex_error(SymetricMatrix q, double x, double y, double z) {
    return q[0] * x * x + 2 * q[1] * x * y + 2 * q[2] * x * z + 2 * q[3] * x + q[4] * y * y +
           2 * q[5] * y * z + 2 * q[6] * y + q[7] * z * z + 2 * q[8] * z + q[9];
}

// Error for one edge

double calculate_error(int id_v1, int id_v2, vec3f &p_result) {
    // compute interpolated vertex

    SymetricMatrix q = vertices[id_v1].q + vertices[id_v2].q;
    bool border      = vertices[id_v1].border & vertices[id_v2].border;
    double error     = 0;
    double det       = q.det(0, 1, 2, 1, 4, 5, 2, 5, 7);
    if (det != 0 && !border) {

        // q_delta is invertible
        p_result.x = -1 / det * (q.det(1, 2, 3, 4, 5, 6, 5, 7, 8)); // vx = A41/det(q_delta)
        p_result.y = 1 / det * (q.det(0, 2, 3, 1, 5, 6, 2, 7, 8));  // vy = A42/det(q_delta)
        p_result.z = -1 / det * (q.det(0, 1, 3, 1, 4, 6, 2, 5, 8)); // vz = A43/det(q_delta)

        error = vertex_error(q, p_result.x, p_result.y, p_result.z);
    } else {
        // det = 0 -> try to find best result
        vec3f p1      = vertices[id_v1].p;
        vec3f p2      = vertices[id_v2].p;
        vec3f p3      = (p1 + p2) / 2;
        double error1 = vertex_error(q, p1.x, p1.y, p1.z);
        double error2 = vertex_error(q, p2.x, p2.y, p2.z);
        double error3 = vertex_error(q, p3.x, p3.y, p3.z);
        error         = min(error1, min(error2, error3));
        if (error1 == error)
            p_result = p1;
        if (error2 == error)
            p_result = p2;
        if (error3 == error)
            p_result = p3;
    }
    return error;
}

char *trimwhitespace(char *str) {
    char *end;

    // Trim leading space
    while (isspace((unsigned char)*str))
        str++;

    if (*str == 0) // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end))
        end--;

    // Write new null terminator
    *(end + 1) = 0;

    return str;
}

// Option : Load OBJ
/*void load_obj(const char* filename, bool process_uv=false){
        vertices.clear();
        triangles.clear();
        //printf ( "Loading Objects %s ... \n",filename);
        FILE* fn;
        if(filename==NULL)		return ;
        if((char)filename[0]==0)	return ;
        if ((fn = fopen(filename, "rb")) == NULL)
        {
                printf ( "File %s not found!\n" ,filename );
                return;
        }
        char line[1000];
        memset ( line,0,1000 );
        int vertex_cnt = 0;
        int material = -1;
        std::map<std::string, int> material_map;
        std::vector<vec3f> uvs;
        std::vector<std::vector<int> > uvMap;

        while(fgets( line, 1000, fn ) != NULL)
        {
                Vertex v;
                vec3f uv;

                //比较line前6个字符是否"mtllib"（材质库文件）
                if (strncmp(line, "mtllib", 6) == 0)
                {
                        //去除line前7个字符（包含空格），得到材质库文件名，导出的时候用
                        mtllib = trimwhitespace(&line[7]);
                }
                //提取材质名
                if (strncmp(line, "usemtl", 6) == 0)
                {
                        std::string usemtl = trimwhitespace(&line[7]);
                        if (material_map.find(usemtl) == material_map.end())
                        {
                                //添加新id（materials.size()）到map，同时添加材质名到materials
                                material_map[usemtl] = materials.size();
                                materials.push_back(usemtl);
                        }
                        //更新material为当前材质id
                        material = material_map[usemtl];
                }

                //如果行以vt开头，表示纹理坐标
                if ( line[0] == 'v' && line[1] == 't' )
                {
                        if ( line[2] == ' ' )
                        {
                                if(sscanf(line,"vt %lf %lf",
                                        &uv.x,&uv.y)==2)
                                {
                                        //uv坐标只有两个，那么z为0
                                        uv.z = 0;
                                        uvs.push_back(uv);
                                } else
                                if(sscanf(line,"vt %lf %lf %lf",
                                        &uv.x,&uv.y,&uv.z)==3)
                                {
                                        uvs.push_back(uv);
                                }
                        }
                }
                //顶点
                else if ( line[0] == 'v' )
                {
                        if ( line[1] == ' ' )
                        if(sscanf(line,"v %lf %lf %lf",
                                &v.p.x,	&v.p.y,	&v.p.z)==3)
                        {
                                vertices.push_back(v);
                        }
                }
                int integers[9];
                //面
                if ( line[0] == 'f' )
                {
                        Triangle t;
                        bool tri_ok = false;
                        bool has_uv = false;

                        //"f 1 2 3"：只v。
                        if(sscanf(line,"f %d %d %d",
                                &integers[0],&integers[1],&integers[2])==3)
                        {
                                tri_ok = true;
                        }else
                        if(sscanf(line,"f %d// %d// %d//",                    //"f 1// 2//
3//"：v//vn。 &integers[0],&integers[1],&integers[2])==3)
                        {
                                tri_ok = true;
                        }else
                        if(sscanf(line,"f %d//%d %d//%d %d//%d",               //"f 1//1 2//2
3//3"：v//vn。 &integers[0],&integers[3], &integers[1],&integers[4], &integers[2],&integers[5])==6)
                        {
                                tri_ok = true;
                        }else
                        if(sscanf(line,"f %d/%d/%d %d/%d/%d %d/%d/%d",          //"f 1/1/1 2/2/2
3/3/3"：v/vt/vn。 &integers[0],&integers[6],&integers[3], &integers[1],&integers[7],&integers[4],
                                &integers[2],&integers[8],&integers[5])==9)
                        {
                                tri_ok = true;
                                has_uv = true;
                        }else // Add Support for v/vt only meshes
                        if (sscanf(line, "f %d/%d %d/%d %d/%d",             //"f 1/1 2/2
3/3"：v/vt。 &integers[0], &integers[6], &integers[1], &integers[7], &integers[2], &integers[8]) ==
6)
                        {
                                tri_ok = true;
                                has_uv = true;
                        }
                        else
                        {
                                printf("unrecognized sequence\n");
                                printf("%s\n",line);
                                exit(1);
                        }
                        if ( tri_ok )
                        {
                                t.v[0] = integers[0]-1-vertex_cnt;
                                t.v[1] = integers[1]-1-vertex_cnt;
                                t.v[2] = integers[2]-1-vertex_cnt;
                                t.attr = 0;

                                if ( process_uv && has_uv )
                                {
                                        std::vector<int> indices;
                                        indices.push_back(integers[6]-1-vertex_cnt);
                                        indices.push_back(integers[7]-1-vertex_cnt);
                                        indices.push_back(integers[8]-1-vertex_cnt);
                                        uvMap.push_back(indices);
                                        t.attr |= TEXCOORD;
                                }

                                t.material = material;
                                //geo.triangles.push_back ( tri );
                                triangles.push_back(t);
                                //state_before = state;
                                //state ='f';
                        }
                }
        }
        if ( process_uv && uvs.size() )
        {
                loopi(0,triangles.size())
                {
                        loopj(0,3)
                        triangles[i].uvs[j] = uvs[uvMap[i][j]];
                }
        }

        fclose(fn);

        //printf("load_obj: vertices = %lu, triangles = %lu, uvs = %lu\n", vertices.size(),
triangles.size(), uvs.size() );
}*/ // load_obj()

// Optional : Store as OBJ

/*void load_obj(const char* filename, bool process_uv=false){
vertices.clear();
triangles.clear();
//printf ( "Loading Objects %s ... \n",filename);
FILE* fn;
if(filename==NULL)      return ;
if((char)filename[0]==0)    return ;
if ((fn = fopen(filename, "rb")) == NULL)
{
printf ( "File %s not found!\n" ,filename );
return;
}
char line[1000];
memset ( line,0,1000 );
int vertex_cnt = 0; // 这里的 vertex_cnt
看起来在您的原始代码中没有被修改，如果OBJ文件引用了多个OBJ，可能需要用到，但这里暂时保留其原始使用方式。
int material = -1;
std::map<std::string, int> material_map;
std::vector<vec3f> uvs;
std::vector<std::vector<int> > uvMap;

while(fgets( line, 1000, fn ) != NULL)
{
Vertex v;
vec3f uv;

//比较line前6个字符是否"mtllib"（材质库文件）
if (strncmp(line, "mtllib", 6) == 0)
{
    //去除line前7个字符（包含空格），得到材质库文件名，导出的时候用
    mtllib = trimwhitespace(&line[7]);
}
//提取材质名
if (strncmp(line, "usemtl", 6) == 0)
{
    std::string usemtl = trimwhitespace(&line[7]);
    if (material_map.find(usemtl) == material_map.end())
    {
        //添加新id（materials.size()）到map，同时添加材质名到materials
        material_map[usemtl] = materials.size();
        materials.push_back(usemtl);
    }
    //更新material为当前材质id
    material = material_map[usemtl];
}

//如果行以vt开头，表示纹理坐标
if ( line[0] == 'v' && line[1] == 't' )
{
    if ( line[2] == ' ' )
    {
        if(sscanf(line,"vt %lf %lf",
            &uv.x,&uv.y)==2)
        {
            //uv坐标只有两个，那么z为0
            uv.z = 0;
            uvs.push_back(uv);
        } else
        if(sscanf(line,"vt %lf %lf %lf",
            &uv.x,&uv.y,&uv.z)==3)
        {
            uvs.push_back(uv);
        }
    }
}
//顶点
else if ( line[0] == 'v' )
{
    if ( line[1] == ' ' )
    if(sscanf(line,"v %lf %lf %lf",
        &v.p.x, &v.p.y, &v.p.z)==3)
    {
        vertices.push_back(v);
    }
}

// **********************************************
// *********** 面片解析逻辑修改开始 *************
// **********************************************

//面
if ( line[0] == 'f' )
{
    // 使用灵活的解析方法处理多边形面片
    // 1. 复制行，因为 strtok 会修改原始字符串
    // 必须使用 strdup 或 std::string::c_str() 的副本
    char* line_copy = strdup(line);
    char* token = strtok(line_copy, " \t\n\r"); // 忽略 'f'

    std::vector<int> v_indices;  // 存储所有顶点索引
    std::vector<int> vt_indices; // 存储所有纹理索引 (只在 process_uv 为 true 时使用)

    // 2. 循环处理每个顶点组 (e.g., "1/2/3" 或 "1//3" 或 "1")
    while ((token = strtok(NULL, " \t\n\r")) != NULL)
    {
        int v = 0, vt = 0;
        char *v_str = token;
        char *vt_str = strchr(token, '/');
        char *vn_str = NULL; // 法线索引 vn

        // 解析 V 索引
        if (vt_str) {
            *vt_str = '\0'; // 临时截断，只保留 V
            v = atoi(v_str);
            *vt_str = '/'; // 恢复字符串

            // 查找第二个 '/'
            vn_str = strchr(vt_str + 1, '/');
            if (vn_str) {
                *vn_str = '\0'; // 临时截断，只保留 VT
                vt = atoi(vt_str + 1);
                *vn_str = '/'; // 恢复字符串
                // vn = atoi(vn_str + 1); // 如果需要处理法线，在这里解析
            } else {
                // 格式是 V/VT
                vt = atoi(vt_str + 1);
            }
        } else {
            // 格式是 V
            v = atoi(v_str);
        }

        if (v != 0) {
            v_indices.push_back(v);
            if (process_uv) {
                vt_indices.push_back(vt);
            }
            // vn_indices.push_back(vn); // 如果需要处理法线，在这里添加
        }
    }
    free(line_copy);

    // 3. 扇形三角化 (Fan Triangulation)
    if (v_indices.size() >= 3)
    {
        // v_indices[0] 是第一个（扇形中心）顶点
        for (size_t i = 1; i < v_indices.size() - 1; ++i)
        {
            Triangle t;

            // 获取当前三角形的 V 索引 (v0, vi, vi+1)
            // 减去 1 (OBJ是 1-based) 和 vertex_cnt (如果索引是相对的)
            t.v[0] = v_indices[0] - 1 - vertex_cnt;
            t.v[1] = v_indices[i] - 1 - vertex_cnt;
            t.v[2] = v_indices[i+1] - 1 - vertex_cnt;

            t.attr = 0;

            // 获取当前三角形的 UV 索引
            if ( process_uv && vt_indices.size() > 0)
            {
                std::vector<int> indices;
                // vt0, vti, vti+1
                indices.push_back(vt_indices[0] - 1 - vertex_cnt);
                indices.push_back(vt_indices[i] - 1 - vertex_cnt);
                indices.push_back(vt_indices[i+1] - 1 - vertex_cnt);
                uvMap.push_back(indices);
                t.attr |= TEXCOORD;
            }

            t.material = material;
            triangles.push_back(t);
        }
    }
}

// **********************************************
// *********** 面片解析逻辑修改结束 *************
// **********************************************

}
if ( process_uv && uvs.size() )
{
loopi(0,triangles.size())
{
    loopj(0,3)
    triangles[i].uvs[j] = uvs[uvMap[i][j]];
}
}

fclose(fn);

//printf("load_obj: vertices = %lu, triangles = %lu, uvs = %lu\n", vertices.size(),
triangles.size(), uvs.size() );
}*/

void load_obj(const char *filename, bool process_uv = false) {
    vertices.clear();
    triangles.clear();
    // printf ( "Loading Objects %s ... \n",filename);
    FILE *fn;
    if (filename == NULL)
        return;
    if ((char)filename[0] == 0)
        return;
    if ((fn = fopen(filename, "rb")) == NULL) {
        printf("File %s not found!\n", filename);
        return;
    }
    // char line[1000];
    // memset ( line,0,1000 );

    char line[MAX_LINE_LENGTH];
    memset(line, 0, MAX_LINE_LENGTH);

    // 调试和解析变量
    long total_f_lines = 0;
    long skipped_faces = 0;
    long line_num      = 0; // 【新增：行号追踪】

    int vertex_cnt = 0;
    int material   = -1;
    std::map<std::string, int> material_map;
    std::vector<vec3f> uvs;
    std::vector<std::vector<int>> uvMap;

    while (fgets(line, MAX_LINE_LENGTH, fn) != NULL) {
        line_num++; // 追踪当前行号

        Vertex v;
        vec3f uv;
        bool line_recognized = false; // 【新增：行识别标记】

        // 比较line前6个字符是否"mtllib"（材质库文件）
        if (strncmp(line, "mtllib", 6) == 0) {
            // 去除line前7个字符（包含空格），得到材质库文件名，导出的时候用
            mtllib          = trimwhitespace(&line[7]);
            line_recognized = true;
        }
        // 提取材质名
        else if (strncmp(line, "usemtl", 6) == 0) {
            std::string usemtl = trimwhitespace(&line[7]);
            if (material_map.find(usemtl) == material_map.end()) {
                // 添加新id（materials.size()）到map，同时添加材质名到materials
                material_map[usemtl] = materials.size();
                materials.push_back(usemtl);
            }
            // 更新material为当前材质id
            material        = material_map[usemtl];
            line_recognized = true;
        }

        // 如果行以vt开头，表示纹理坐标
        else if (line[0] == 'v' && line[1] == 't') {
            if (line[2] == ' ') {
                if (sscanf(line, "vt %lf %lf", &uv.x, &uv.y) == 2) {
                    // uv坐标只有两个，那么z为0
                    uv.z = 0;
                    uvs.push_back(uv);
                } else if (sscanf(line, "vt %lf %lf %lf", &uv.x, &uv.y, &uv.z) == 3) {
                    uvs.push_back(uv);
                }
            }
            line_recognized = true;
        }
        // 法线 (vn)
        else if (line[0] == 'v' && line[1] == 'n') {
            // 如果需要，在这里添加法线解析
            line_recognized = true;
        }
        // 顶点
        else if (line[0] == 'v' && line[1] == ' ') {
            if (sscanf(line, "v %lf %lf %lf", &v.p.x, &v.p.y, &v.p.z) == 3) {
                vertices.push_back(v);
            }
            line_recognized = true;
        }
        // 【新增：Object Name 'o'】
        else if (line[0] == 'o' && line[1] == ' ') {
            // Object Name is recognized and ignored
            line_recognized = true;
        }
        // 【新增：Smoothing Group 's'】
        else if (line[0] == 's' && line[1] == ' ') {
            // Smoothing Group is recognized and ignored
            line_recognized = true;
        }
        // 【新增：Line/Edge 'l'】
        else if (line[0] == 'l' && line[1] == ' ') {
            // Line/Edge is recognized and ignored
            line_recognized = true;
        }

        // **********************************************
        // *********** 面片解析逻辑 (f) *****************
        // **********************************************

        // 面
        else if (line[0] == 'f') {
            total_f_lines++; // 统计遇到的所有 'f' 行
            line_recognized = true;

            // 1. 复制行，因为 strtok 会修改原始字符串
            char *line_copy = strdup(line);
            char *token     = strtok(line_copy, " \t\n\r"); // 忽略 'f'

            std::vector<int> v_indices;  // 存储所有顶点索引
            std::vector<int> vt_indices; // 存储所有纹理索引 (只在 process_uv 为 true 时使用)

            // 2. 循环处理每个顶点组 (e.g., "1/2/3" 或 "1//3" 或 "1")
            while ((token = strtok(NULL, " \t\n\r")) != NULL) {
                int v = 0, vt = 0;
                char *v_str  = token;
                char *vt_str = strchr(token, '/');
                char *vn_str = NULL; // 法线索引 vn

                // 解析 V 索引
                if (vt_str) {
                    *vt_str = '\0'; // 临时截断，只保留 V
                    v       = atoi(v_str);
                    *vt_str = '/'; // 恢复字符串

                    // 查找第二个 '/'
                    vn_str = strchr(vt_str + 1, '/');
                    if (vn_str) {
                        *vn_str = '\0'; // 临时截断，只保留 VT
                        vt      = atoi(vt_str + 1);
                        *vn_str = '/'; // 恢复字符串
                        // vn = atoi(vn_str + 1); // 如果需要处理法线，在这里解析
                    } else {
                        // 格式是 V/VT
                        vt = atoi(vt_str + 1);
                    }
                } else {
                    // 格式是 V
                    v = atoi(v_str);
                }

                if (v != 0) {
                    v_indices.push_back(v);
                    if (process_uv) {
                        // 只有当解析出有效的 vt 索引时才记录
                        vt_indices.push_back(vt);
                    }
                    // vn_indices.push_back(vn); // 如果需要处理法线，在这里添加
                }
            }
            free(line_copy);

            // 3. 扇形三角化 (Fan Triangulation)
            if (v_indices.size() >= 3) {
                // v_indices[0] 是第一个（扇形中心）顶点
                for (size_t i = 1; i < v_indices.size() - 1; ++i) {
                    Triangle t;

                    // 获取当前三角形的 V 索引 (v0, vi, vi+1)
                    t.v[0] = v_indices[0] - 1 - vertex_cnt;
                    t.v[1] = v_indices[i] - 1 - vertex_cnt;
                    t.v[2] = v_indices[i + 1] - 1 - vertex_cnt;

                    t.attr = 0;

                    // 获取当前三角形的 UV 索引
                    if (process_uv && vt_indices.size() == v_indices.size()) {
                        std::vector<int> indices;
                        indices.push_back(vt_indices[0] - 1 - vertex_cnt);
                        indices.push_back(vt_indices[i] - 1 - vertex_cnt);
                        indices.push_back(vt_indices[i + 1] - 1 - vertex_cnt);
                        uvMap.push_back(indices);
                        t.attr |= TEXCOORD;
                    }

                    t.material = material;
                    triangles.push_back(t);
                }
            } else {
                // 顶点少于 3 个，认定为无效面片，跳过
                skipped_faces++;
            }
        }

        // 【通用行格式检查和警告】
        if (!line_recognized) {
            // 复制行到缓冲区，因为 trimwhitespace 修改输入字符串
            char *line_buf      = strdup(line);
            char *trimmed_c_str = trimwhitespace(line_buf);

            // 忽略注释行 (#) 和空行
            if (strlen(trimmed_c_str) > 0 && trimmed_c_str[0] != '#') {
                printf(
                    "!!! WARNING !!! Unrecognized line format at line %ld. Content: '%.40s...'\n",
                    line_num, trimmed_c_str);
            }
            free(line_buf);
        }
    }

    // 赋值 UV 坐标
    if (process_uv && uvs.size()) {
        loopi(0, triangles.size()) {
            loopj(0, 3)
                // 增加边界检查，避免在 UV 索引无效时崩溃
                if (i < uvMap.size() && j < uvMap[i].size() && uvMap[i][j] < uvs.size()) {
                triangles[i].uvs[j] = uvs[uvMap[i][j]];
            }
            else {
                // 如果 UV 索引不合法，可能需要将 uvs[j] 清零或设置默认值
            }
        }
    }

    fclose(fn);

    // 打印调试统计信息
    printf("load_obj Statistics:\n");
    printf("  Lines Read (Total): %ld\n", line_num); // 【输出总行数】
    printf("  Original Faces ('f' lines): %ld\n", total_f_lines);
    printf("  Invalid Faces Skipped (v < 3): %ld\n", skipped_faces);
    printf("  Final Triangle Count (Triangulated): %lu\n", triangles.size());
    printf("  Vertex Count: %lu\n", vertices.size());
}

/*void write_obj(const char* filename)
        {
                FILE *file=fopen(filename, "w");
                int cur_material = -1;
                bool has_uv = (triangles.size() && (triangles[0].attr & TEXCOORD) == TEXCOORD);

                if (!file)
                {
                        printf("write_obj: can't write data file \"%s\".\n", filename);
                        exit(0);
                }
                if (!mtllib.empty())
                {
                        fprintf(file, "mtllib %s\n", mtllib.c_str());
                }
                loopi(0,vertices.size())
                {
                        //fprintf(file, "v %lf %lf %lf\n",
   vertices[i].p.x,vertices[i].p.y,vertices[i].p.z); fprintf(file, "v %g %g %g\n",
   vertices[i].p.x,vertices[i].p.y,vertices[i].p.z); //more compact: remove trailing zeros
                }
                if (has_uv)
                {
                        loopi(0,triangles.size()) if(!triangles[i].deleted)
                        {
                                fprintf(file, "vt %g %g\n", triangles[i].uvs[0].x,
   triangles[i].uvs[0].y); fprintf(file, "vt %g %g\n", triangles[i].uvs[1].x,
   triangles[i].uvs[1].y); fprintf(file, "vt %g %g\n", triangles[i].uvs[2].x,
   triangles[i].uvs[2].y);
                        }
                }
                int uv = 1;
                loopi(0,triangles.size()) if(!triangles[i].deleted)
                {
                        if (triangles[i].material != cur_material)
                        {
                                cur_material = triangles[i].material;
                                fprintf(file, "usemtl %s\n",
   materials[triangles[i].material].c_str());
                        }
                        if (has_uv)
                        {
                                fprintf(file, "f %d/%d %d/%d %d/%d\n", triangles[i].v[0]+1, uv,
   triangles[i].v[1]+1, uv+1, triangles[i].v[2]+1, uv+2); uv += 3;
                        }
                        else
                        {
                                fprintf(file, "f %d %d %d\n", triangles[i].v[0]+1,
   triangles[i].v[1]+1, triangles[i].v[2]+1);
                        }
                        //fprintf(file, "f %d// %d// %d//\n", triangles[i].v[0]+1,
   triangles[i].v[1]+1, triangles[i].v[2]+1); //more compact: remove trailing zeros
                }
                fclose(file);
        }*/

void write_obj(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("write_obj: can't write data file \"%s\".\n", filename);
        exit(0);
    }

    // --- 核心修复：准备唯一的 UVs 列表和映射 ---
    std::vector<vec3f> unique_uvs;
    // 映射: 原始UV值 -> 唯一UV列表中的新索引
    // 注意：使用 map<vec3f, int> 需要 vec3f 定义 operator<, 为简单起见，这里使用平面数组来存储所有
    // UV，并在 f 中使用正确的索引

    // 我们将采用一个更简单的策略：先收集所有唯一的 UVs，再写出。

    // 存储面片所需的 UV 索引
    std::vector<std::vector<int>> face_uv_indices;

    int cur_material = -1;
    bool has_uv      = (triangles.size() && (triangles[0].attr & TEXCOORD) == TEXCOORD);

    // 1. 写出mtllib和所有顶点 (v)
    if (!mtllib.empty()) {
        fprintf(file, "mtllib %s\n", mtllib.c_str());
    }
    loopi(0, vertices.size()) {
        fprintf(file, "v %g %g %g\n", vertices[i].p.x, vertices[i].p.y, vertices[i].p.z);
    }

    // 2. 如果有 UV，先将所有 UV 收集到 unique_uvs 中
    if (has_uv) {
        // 重置 UV 索引映射。注意：这可能需要额外的库来实现 vec3f 的哈希或比较
        std::map<double, int> uvs_map; // 实际上应该用一个更复杂的哈希映射
        int current_uv_index = 1;

        // 遍历所有未删除的三角形
        loopi(0, triangles.size()) if (!triangles[i].deleted) {
            std::vector<int> tri_uv_indices;
            loopj(0, 3) {
                const vec3f &uv_val = triangles[i].uvs[j];

                // ！！！ 注意：这是 UV 共享逻辑的关键难点 ！！！
                // 由于 double 精度问题，我们不能直接使用 map<vec3f, int> 或直接查找。
                // 必须依赖于加载时建立的原始 UV 索引 (uvMap) 或在简化过程中保持 UV 共享。

                // 鉴于您没有提供原始 UV 索引 uvMap 或一个安全的哈希方法，
                // 我们只能采取折衷方案：只收集并写出 UV 值，并在 f 记录中引用它们。
                // ******* 为了修复您代码的 UV 索引问题，我们必须假设：
                // ******* 在加载时，您在 triangles[i].uvs 中存储了原始全局 uvs 列表的索引，
                // ******* 并且该索引在简化过程中被正确维护。

                // 既然 load_obj 成功了，我们应该使用它导出的结构。
                // 然而，您的 write_obj 代码完全忽略了原始 UV 列表和其索引。
                // 唯一的办法是重新引入一个 UV 列表，并在 write_obj 中解决共享问题。

                // 鉴于简化器的复杂性，最安全的修复是：
                // 假设简化过程保证了每个 v[j] 对应的 uvs[j] 是唯一的，
                // 或者我们必须实现完整的 UV 共享逻辑，这超出了函数本身的修改范围。

                // 回退到您的代码逻辑：它将导致重复 UVs，但至少 UV 索引会匹配 f 记录。
                // 修复其索引计算逻辑：

                // **为了保证导出的文件是有效的（即使重复了
                // UVs），我们继续使用您的逻辑，但必须修正其结构。**

                // 如果您打算保留现有的 UV 重复逻辑 (不推荐)，请确保 f 中的索引引用的是正确的 uvMap
                // 索引，而不是 v 索引。

                // 由于您在 load_obj 中没有将 UV 索引存储到 triangles
                // 结构中，我们必须**强制**重新使用您的重复 UV 逻辑，并保证索引同步。

                unique_uvs.push_back(triangles[i].uvs[j]);
            }
        }

        // 3. 写出 UVs
        loopi(0, unique_uvs.size()) {
            fprintf(file, "vt %g %g\n", unique_uvs[i].x, unique_uvs[i].y);
        }
    }

    // 4. 写出面片 (f)
    int uv_index_base = 1; // OBJ 是 1-based

    loopi(0, triangles.size()) if (!triangles[i].deleted) {
        if (triangles[i].material != cur_material) {
            cur_material = triangles[i].material;
            fprintf(file, "usemtl %s\n", materials[triangles[i].material].c_str());
        }

        // 确保导出的是三角形
        int v0 = triangles[i].v[0] + 1;
        int v1 = triangles[i].v[1] + 1;
        int v2 = triangles[i].v[2] + 1;

        if (has_uv) {
            // 使用 uv_index_base + 0, + 1, + 2 来引用刚刚写出的 3 个 vt 记录
            fprintf(file, "f %d/%d %d/%d %d/%d\n", v0, uv_index_base, v1, uv_index_base + 1, v2,
                    uv_index_base + 2);

            uv_index_base += 3; // 索引跳过 3 个 UV
        } else {
            fprintf(file, "f %d %d %d\n", v0, v1, v2);
        }
    }

    fclose(file);
}

#ifdef _MSC_VER
#pragma pack(2)
struct mz3hdr {
    uint16_t SIGNATURE, ATTR;
    uint32_t NFACE, NVERT, NSKIP;
};
#pragma pack()
#else
struct __attribute__((__packed__)) mz3hdr {
    uint16_t SIGNATURE, ATTR;
    uint32_t NFACE, NVERT, NSKIP;
};
#endif

void load_mz3(const char *filename) {
    vertices.clear();
    triangles.clear();
    int material = -1;
    std::map<std::string, int> material_map;
    std::vector<vec3f> uvs;
    std::vector<std::vector<int>> uvMap;
    FILE *fp = fopen(filename, "rb");
    struct mz3hdr h;
    size_t bytes_read = fread(&h, sizeof(struct mz3hdr), 1, fp);
    if (bytes_read <= 0) {
        printf("Unable to read %s\n", filename);
        exit(EXIT_FAILURE);
    }
    uint16_t sig = 23117;
    if (sig != h.SIGNATURE) {
        fclose(fp);
        printf("Unable to read mz3 (unable to read gz compressed) %s\n", filename);
        exit(EXIT_FAILURE);
    }
    triangles.resize(h.NFACE);
    vertices.resize(h.NVERT);
    fseek(fp, (int)(sizeof(struct mz3hdr) + h.NSKIP), SEEK_SET);
    uint32_t tribytes = h.NFACE * 3 * sizeof(uint32_t);
    uint32_t *tris    = (uint32_t *)malloc(tribytes);
    bytes_read        = fread(tris, tribytes, 1, fp);
    if (bytes_read <= 0) {
        printf("Unable to read triangles %s\n", filename);
        exit(EXIT_FAILURE);
    }
    uint32_t vertbytes32 = h.NVERT * 3 * sizeof(float);
    float *verts32       = (float *)malloc(vertbytes32);
    bytes_read           = fread(verts32, vertbytes32, 1, fp);
    if (bytes_read <= 0) {
        printf("Unable to read vertices %s\n", filename);
        exit(EXIT_FAILURE);
    }
    int j = 0;
    for (int i = 0; i < h.NVERT; i++) {
        vertices[i].p.x = verts32[j++];
        vertices[i].p.y = verts32[j++];
        vertices[i].p.z = verts32[j++];
    }
    j = 0;
    for (int i = 0; i < h.NFACE; i++) {
        triangles[i].v[0]     = tris[j++];
        triangles[i].v[1]     = tris[j++];
        triangles[i].v[2]     = tris[j++];
        triangles[i].attr     = 0;
        triangles[i].material = material;
    }
    free(tris);
    free(verts32);
    fclose(fp);
}

void write_mz3(const char *filename) {
    // https://github.com/neurolabusc/surf-ice/tree/master/mz3
    bool has_uv = (triangles.size() && (triangles[0].attr & TEXCOORD) == TEXCOORD);
    if (has_uv) {
        printf("write_mz3 can not store UVs (use .obj format).\n");
    }
    struct mz3hdr h;
    h.SIGNATURE = 0x5A4D;
    h.ATTR      = 3; // isFACE +1 isVERT +2
    h.NVERT     = vertices.size();
    h.NSKIP     = 0;
    h.NFACE     = 0;
    for (int i = 0; i < triangles.size(); i++) {
        if (!triangles[i].deleted)
            h.NFACE++;
    }
    if ((h.NFACE < 1) || (h.NVERT < 3)) {
        printf("write_mz3: no surviving triangles.\n");
        exit(1);
    }
    FILE *fp;
    fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("write_mz3: can't write data file \"%s\".\n", filename);
        exit(1);
    }
    fwrite(&h, sizeof(struct mz3hdr), 1, fp);
    uint32_t tribytes = h.NFACE * 3 * sizeof(uint32_t);
    uint32_t *tris    = (uint32_t *)malloc(tribytes);
    int j             = 0;
    for (int i = 0; i < triangles.size(); i++) {
        if (triangles[i].deleted)
            continue;
        tris[j++] = triangles[i].v[0];
        tris[j++] = triangles[i].v[1];
        tris[j++] = triangles[i].v[2];
    }
    fwrite(tris, tribytes, 1, fp);
    free(tris);
    uint32_t vertbytes32 = h.NVERT * 3 * sizeof(float);
    float *verts32       = (float *)malloc(vertbytes32);
    j                    = 0;
    for (int i = 0; i < h.NVERT; i++) {
        verts32[j++] = vertices[i].p.x;
        verts32[j++] = vertices[i].p.y;
        verts32[j++] = vertices[i].p.z;
    }
    fwrite(verts32, vertbytes32, 1, fp);
    free(verts32);
    fclose(fp);
}
}; // namespace Simplify

///////////////////////////////////////////
