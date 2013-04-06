void OpenGLInit(int argc, char** argv);

//class Point
//{
    //public:
    //GLfloat x, y, z;
    //Point();
    //Point(GLfloat x, GLfloat y, GLfloat z);
    //void Assign(GLfloat x, GLfloat y, GLfloat z);
//};

//Point::Point()
//{
    //this->x = this->y = this->z = 0;
//}
//Point::Point(GLfloat x, GLfloat y, GLfloat z)
//{
    //this->x = x;
    //this->y = y;
    //this->z = z;
//}
//void Point::Assign(GLfloat x, GLfloat y, GLfloat z)
//{
    //this->x = x;
    //this->y = y;
    //this->z = z;
//}


//class Vector
//{
    //public:
    //GLfloat x, y, z;
    //Vector();
    //Vector(GLfloat x, GLfloat y, GLfloat z);
    //Vector(Point p);
    //void Assign(GLfloat x, GLfloat y, GLfloat z);
    //void AddSub(Vector v1, Vector v2, bool add);
    //void CrossProduct(Vector v1, Vector v2);
    //GLfloat DotProduct(Vector v);
    //GLfloat Length();
    //void Normalize();
//};

//Vector::Vector(Point p)
//{
    //this->x = p.x;
    //this->y = p.y;
    //this->z = p.z;
//}
//Vector::Vector()
//{
    //this->x = this->y = this->z = 0;
//}
//Vector::Vector(GLfloat x, GLfloat y, GLfloat z)
//{
    //this->x = x;
    //this->y = y;
    //this->z = z;
//}
//void Vector::Assign(GLfloat x, GLfloat y, GLfloat z)
//{
    //this->x = x;
    //this->y = y;
    //this->z = z;
//}
//void Vector::CrossProduct(Vector v1, Vector v2)
//{
    //this->x = v1.y*v2.z - v1.z*v2.y;
    //this->y = v1.z*v2.x - v1.x*v2.z;
    //this->z = v1.x*v2.y - v2.x*v1.y;
//}
//void Vector::AddSub(Vector v1, Vector v2, bool add)
//{
    //short i;
    //if(add)
    //{
        //this->x = v1.x + v2.x;
        //this->y = v1.y + v2.y;
        //this->z = v1.z + v2.z;
    //}
    //else
    //{
        //this->x = v1.x - v2.x;
        //this->y = v1.y - v2.y;
        //this->z = v1.z - v2.z;
    //}
//}
//GLfloat Vector::DotProduct(Vector v)
//{
    //return (((this->x)*v.x)+((this->y)*v.y)+((this->z)*v.z));
//}
//GLfloat Vector::Length()
//{
    //return(this->x*this->x+this->y*this->y+this->z*this->z);
//}
//void Vector::Normalize()
//{
    //GLfloat len = this->Length();
    //GLfloat recip = 0;
    //if(len == 0)
        //this->Assign(0, 0, 0);
    //else
    //{
        //recip = 1/sqrt(len);
        //this->x *= recip;
        //this->y *= recip;
        //this->z *= recip;
    //}
//}
