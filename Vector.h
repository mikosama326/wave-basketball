#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

class Vector
{
     public:
     float xcomp, ycomp;

     Vector();
     Vector(float x, float y);
     void setvals(float x, float y);//actually no longer necessary. But whatever.
     float getxcomp();//actually no longer necessary. But whatever.
     float getycomp();//actually no longer necessary. But whatever.
     float magnitude();
     Vector operator+(Vector b);
     Vector operator-(Vector b);
     Vector operator*(float scalar);
     Vector operator/(float scalar);
     Vector operator+(const float val[2]);
     void adjust();
};

#endif // VECTOR_H_INCLUDED
