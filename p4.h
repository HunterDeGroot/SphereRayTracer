// prof Montoya allowed us to use these template classes

template<typename T>
class Vec3
{
public:
    T x, y, z;
    Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
    Vec3(T xx) : x(xx), y(xx), z(xx) {}
    Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
    Vec3& normalize()
    {
        T nor2 = length2();
        if (nor2 > 0) {
            T invNor = 1 / sqrt(nor2);
            x *= invNor, y *= invNor, z *= invNor;
        }
        return *this;
    }
    Vec3<T> operator * (const T &f) const { return Vec3<T>(x * f, y * f, z * f); }
    Vec3<T> operator * (const Vec3<T> &v) const { return Vec3<T>(x * v.x, y * v.y, z * v.z); }
    T dot(const Vec3<T> &v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3<T> operator - (const Vec3<T> &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
    Vec3<T> operator + (const Vec3<T> &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
    Vec3<T>& operator += (const Vec3<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
    Vec3<T>& operator *= (const Vec3<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
    Vec3<T> operator - () const { return Vec3<T>(-x, -y, -z); }
    T length2() const { return x * x + y * y + z * z; }
    T length() const { return sqrt(length2()); }
    friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v)
    {
        os << "[" << v.x << " " << v.y << " " << v.z << "]";
        return os;
    }
};

template<typename T>
class Sphere
{
public:
    Vec3<T> center;                         /// position of the sphere
    T radius, radius2;                      /// sphere radius and radius^2
    Vec3<T> surfaceColor, emissionColor;    /// surface color and emission (light)
    T transparency, reflection;             /// surface transparency and reflectivity
    Sphere(const Vec3<T> &c, const T &r, const Vec3<T> &sc,
           const T &refl = 0, const T &transp = 0, const Vec3<T> &ec = 0) :
    center(c), radius(r), radius2(r * r), surfaceColor(sc), emissionColor(ec),
    transparency(transp), reflection(refl)
    {}
    // compute a ray-sphere intersection using the geometric solution
    bool intersect(const Vec3<T> &rayorig, const Vec3<T> &raydir, T *t0 = NULL, T *t1 = NULL) const
    {
        Vec3<T> l = center - rayorig;
        T tca = l.dot(raydir);
        if (tca < 0) return false;
        T d2 = l.dot(l) - tca * tca;
        if (d2 > radius2) return false;
        T thc = sqrt(radius2 - d2);
        if (t0 != NULL && t1 != NULL) {
            *t0 = tca - thc;
            *t1 = tca + thc;
        }
        
        return true;
    }
};
