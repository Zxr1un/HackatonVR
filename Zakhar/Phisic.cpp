

#include <iostream>
#include <vector>
#include <windows.h>

using namespace std;
class object {
public:
    string name = "uncknown";
    double x = 0;
    double y = 0;
    double z = 0;

    double vx = 0;
    double vy = 0;
    double vz = 0;

    double mass = 1;
    double rad = 1;

    double prx = x;
    double pry = y;
    double prz = z;


    object(double x = 0,
    double y = 0,
    double z = 0,
    double vx = 0,
    double vy = 0,
    double vz = 0,
    double mass = pow(10,11),
    double rad = 1,
    string name = "uncknown")
    {
        this->name = name;
        this->mass = mass;
        this->rad = rad;
        this->x = x;
        this->y = y;
        this->z = z;
        this->vx = vx;
        this->vy = vy;
        this->vz = vz;

    }
};

class objects {
public:
    double G = 6.07 * pow(10,-11);
    vector<object> vec;
    int ammount = 0;
    double SpF = 0.25;
    void physics() {

        for (int i = 0; i < vec.size(); i++) {
            vec[i].prx = vec[i].x;
            vec[i].pry = vec[i].y;
            vec[i].prz = vec[i].z;
            double a = 0;
            double ax = 0;
            double ay = 0;
            double az = 0;
            
            double d2 = 0;
            double dx = 0;
            double dy = 0;
            double dz = 0;
            double dist = 0;


            for (int j = 0; j < vec.size(); j++) {
                if (j != i) {
                    dx = vec[j].x - vec[i].x;
                    dy = vec[j].y - vec[i].y;
                    dz = vec[j].z - vec[i].z;
                    d2 = dx * dx + dy * dy + dz * dz;
                                       
                    a = G * vec[j].mass / d2;

                    
                    ax += a * dx / pow(d2, 0.5);
                    ay += a * dy / pow(d2, 0.5);
                    az += a * dz / pow(d2, 0.5);
                }
            }
            vec[i].prx += vec[i].vx * SpF + 0.5 * ax * SpF * SpF;
            vec[i].pry += vec[i].vy * SpF + 0.5 * ay * SpF * SpF;
            vec[i].prz += vec[i].vz * SpF + 0.5 * az * SpF * SpF;

            vec[i].vx += ax * SpF;
            vec[i].vy += ay * SpF;
            vec[i].vz += az * SpF;

        }

        for (int i = 0; i < vec.size(); i++) {
            vec[i].x = vec[i].prx;
            vec[i].y = vec[i].pry;
            vec[i].z = vec[i].prz;
        }
    }
    void add(double x = 0,
        double y = 0,
        double z = 0,
        double vx = 0,
        double vy = 0,
        double vz = 0,
        double mass = 1,
        double rad = 1,
        string name = "uncknown") {
        vec.push_back( object(x,y,z,vx,vy,vz,mass,rad, name));
        ammount++;
    }
    void print() {

        for (int i = 0; i < vec.size(); i++) {
            
            cout << endl << endl << vec[i].name;
            cout << "\nPosition: " << vec[i].x << " " << vec[i].y << " " << vec[i].z << " ";
            cout << "\nSpeed:    " << vec[i].vx << " " << vec[i].vy << " " << vec[i].vz << " ";

            cout << endl;

        }
        
    }
};




int main()
{
    
    objects all1;
    cout << all1.G << endl;
    all1.add(0,5,0,  1.2,0,0, pow(1,1), 1, "Earth");
    all1.add(0, 0, 0,  0,0,0,   pow(10, 11), 1, "Earth2");
    all1.print();
    char screen[10][10];
    
    for (int k = 0; k < 1000; k++) {
        cout << "\n\t Iter " << k << "\n";
        for (int j = 0; j < 4; j++) {
            all1.physics();
        }
        for (int i = 0; i < 30; i++) {
            for (int j = 0; j < 30; j++) {
                if ((int)all1.vec[0].x + 15 == j and (int)all1.vec[0].y + 15 == i) {
                    cout << "@ ";

                }
                else if ((int)all1.vec[1].x + 15 == j and (int)all1.vec[1].y + 15 == i) {
                    cout << "* ";

                }
                else cout << "  ";
            }
            
            cout << endl;
        }
        all1.print();
        system("pause");
    }
    
    
}

