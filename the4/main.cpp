#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "dynamicgrid.h"


using namespace std;

void testcase1() {
    DynamicGrid<int> g;
    g(2,5) = 42;
    cout << g;
    cout << ~g << "\n";
    cout << g.GetIndex(2,5) << " " << g.GetIndex(0,0) << "\n";
}
/*
testcase1
         0         0         0         0
         0         0         0         0
         0         0         0         0         0        42
14
42 0
*/

void testcase2() {
    DynamicGrid<int> g(3,4);
    g.SetIndex(0,0,7);
    g.SetIndex(1,2,5);
    g.SetIndex(2,3,9);
    g.SetIndex(1,999,123);
    cout << g;
    cout << ~g << "\n";
    cout << g.GetIndex(1,999) << "\n";
}
/*
testcase2
         7         0         0         0
         0         0         5         0
         0         0         0         9
12
0
*/

void testcase3() {
    DynamicGrid<int> g;
    g ^ 3;             // width defaults to 4
    cout << g;
    g(0,6) = 1;        // width becomes 7
    g ^ 9;             // append row width 7
    cout << g;
}
/* 
testcase3
         3         3         3         3
         3         3         3         3         0         0         1
         9         9         9         9         9         9         9
*/

void testcase4() {
    DynamicGrid<int> g(2,6);
    for (int j=0;j<6;++j) g.SetIndex(0, j, j%2? 0: j);
    for (int j=0;j<6;++j) g.SetIndex(1,j, j%3? 1: 0);
    cout << g;
}
/*
testcase4
         0         0         2         0         4         0
         0         1         1         0         1         1
*/

void testcase5() {
    DynamicGrid<int> g;
    g(0,2) = 10;
    g(1,6) = 20;
    cout << g;
    ++g;
    cout << g;
}
/* 
testcase5
         0         0        10         0
         0         0         0         0         0         0        20
         0         0        10         0         0         0         0
         0         0         0         0         0         0        20
*/

void testcase6() {
    DynamicGrid<int> g(2,5);
    cout << g << ~g << "\n";
    g % 0;
    cout << g << ~g << "\n";
}
/*
testcase6
         0         0         0         0         0
         0         0         0         0         0
10


0
*/

void testcase7() {
    DynamicGrid<int> a;
    cout<<~a<<endl;
    a(0,0)=11; a(0,1)=22; a(2,3)=33;
    cout<<~a<<endl;
    DynamicGrid<int> b(a);
    DynamicGrid<int> c; c = a;
    b(0,0)=999;
    cout << a << b << c;
    a = a;
    cout << a;
}
/*
testcase7
0
12
        11        22         0         0
         0         0         0         0
         0         0         0        33
       999        22         0         0
         0         0         0         0
         0         0         0        33
        11        22         0         0
         0         0         0         0
         0         0         0        33
        11        22         0         0
         0         0         0         0
         0         0         0        33
*/



void testcase8() {
    DynamicGrid<int> g;
    g(1,1) = 5;
    int& r = g(1,1);
    r += 10;
    cout << g << g.GetIndex(1,1) << "\n";
}
/*
testcase8
         0         0         0         0
         0        15         0         0
15
*/

void testcase9() {
    istringstream is(
        "1 2 3 4\n"
        "5 6 7 8\n"
        "9 10 11 12\n"
    );
    DynamicGrid<int> d;
    is >> d;
    cout << d << ~d << "\n";
}
/* 
testcase9
         1         2         3         4
         5         6         7         8
         9        10        11        12
12
*/

void testcase10() {
    istringstream is(
        "-39    0 -46 13     12    37 -43	     50     -2\n"
        "-27     -7    15  -15   29     -37	     -11     9\n"
        "\n"
        "\n"
        "22  -33     -50	 -43  19\n"
        "48     -7  30  8  -23\n"
        "-34 -44    -20  20  32   -4\n"
        "-2     8	 -30  49 -17\n"
        "0    -19   35 -37 -41\n"
        "-18  44   -6    10   45\n"
        "-50    -2	 0 -38   -23\n"
        "-3   -36 -27  -48  37\n"
    );
    
    DynamicGrid<int> d;
    is >> d;
    cout << d << ~d << "\n";
    cout << d.GetIndex(0,4) << "\n";
}

/* 
testcase10
       -39         0       -46        13        12        37       -43        50        -2
       -27        -7        15       -15        29       -37       -11         9         0
         0         0         0         0         0         0         0         0         0
         0         0         0         0         0         0         0         0         0
        22       -33       -50       -43        19         0         0         0         0
        48        -7        30         8       -23         0         0         0         0
       -34       -44       -20        20        32        -4         0         0         0
        -2         8       -30        49       -17         0         0         0         0
         0       -19        35       -37       -41         0         0         0         0
       -18        44        -6        10        45         0         0         0         0
       -50        -2         0       -38       -23         0         0         0         0
        -3       -36       -27       -48        37         0         0         0         0
108
12
*/


void testcase11() {
    string fname;
    cin >> fname; //double1.txt
    ifstream f(fname.c_str());
    DynamicGrid<double> gd;
    DynamicGrid<string> gs;
    f >> gd;
    f.clear(); f.seekg(0);
    f >> gs;
    f.close();
    cout <<gd << ~gd << "\n";
    cout << gs << ~gs << "\n";
    gd % -34.97; ++gd;gd ^ 6.66;
    gs % string(""); ++gs; gs ^ string("X");
    cout << gd << ~gd << "\n";
    cout << gs << ~gs << "\n";
}

/* 
testcase11
double1.txt
     -4.85    -40.02    -12.56      6.35     11.51
    -25.79      1.74    -33.87     25.71     29.86
    -34.97     -44.8      -0.5    -48.77         0
    -47.46    -45.55    -45.12     -5.58     -2.46
     44.87     43.39    -34.97     21.56         0
     10.44    -34.97     -43.1    -34.97         0
    -34.97      25.6     20.82    -38.74         0
    -19.21    -34.97     46.33    -33.83       6.9
40
     -4.85    -40.02    -12.56      6.35     11.51
    -25.79      1.74    -33.87     25.71     29.86
    -34.97    -44.80     -0.50    -48.77          
    -47.46    -45.55    -45.12     -5.58     -2.46
     44.87     43.39    -34.97     21.56          
     10.44    -34.97    -43.10    -34.97          
    -34.97     25.60     20.82    -38.74          
    -19.21    -34.97     46.33    -33.83      6.90
40
     -4.85    -40.02    -12.56      6.35     11.51
    -25.79      1.74    -33.87     25.71     29.86
     -44.8      -0.5    -48.77         0         0
    -47.46    -45.55    -45.12     -5.58     -2.46
     44.87     43.39     21.56         0         0
     10.44     -43.1         0         0         0
      25.6     20.82    -38.74         0         0
    -19.21     46.33    -33.83       6.9         0
      6.66      6.66      6.66      6.66      6.66
45
     -4.85    -40.02    -12.56      6.35     11.51
    -25.79      1.74    -33.87     25.71     29.86
    -34.97    -44.80     -0.50    -48.77          
    -47.46    -45.55    -45.12     -5.58     -2.46
     44.87     43.39    -34.97     21.56          
     10.44    -34.97    -43.10    -34.97          
    -34.97     25.60     20.82    -38.74          
    -19.21    -34.97     46.33    -33.83      6.90
         X         X         X         X         X
45
*/

void testcase12() {
    string fchars, fwords;
    cin >> fchars >> fwords;

    ifstream fc(fchars.c_str()), fw(fwords.c_str());

    DynamicGrid<char>    A;
    DynamicGrid<string>  B;

    fc >> A;fc.close();
    fw >> B;fw.close();

    A ^ 'Z';
    A(3,10) = 'K';
    A % 'Z';
    ++A;
    B ^ "SS";

    cout << A << ~A << "\n";
    cout << B << ~B << "\n";
    B % "apple";
    cout << B << ~B << "\n";
}

/*
testcase12
char1.txt
string2.txt
         K         m         P         P         U         B         p         m                           
         Y         Y         d         k         K         B         M                                    
         A         m         h         j         W         x                                             
         N         L         b         p         l         r         S                                    K
         g         f         v         X         n                                                      
         z         P         q         w                                                               
         e         m         t         K         R         y                                             
         O         u         n         C         i                                                      
                                                                                                   
99
     apple     apple      keep      test     stone
      tree       sky     apple     count          
     apple     apple     apple     apple     apple
      grid     happy    yellow     apple          
     stone     apple       dog       sky     apple
       run      tree      code                    
     apple      grid     apple     stone          
     count    yellow       run     thing          
        SS        SS        SS        SS        SS
45
      keep      test     stone
      tree       sky     count          

      grid     happy    yellow          
     stone       dog       sky
       run      tree      code                    
      grid     stone          
     count    yellow       run     thing          
        SS        SS        SS        SS        SS
32
*/

void testcase13() {
    DynamicGrid<string> w;
    w ^ string("x");
    w ^ string("SS");
    w(1,3) = "keep";
    cout << w;
    ++w;
    cout << w << ~w << "\n";
}

/* 
testcase13
         x         x         x         x
        SS        SS        SS      keep
         x         x         x         x
        SS        SS        SS      keep
8
*/


void testcase14() {
    DynamicGrid<char> g;
    g(0,30) = 'Z';
    cout << g << ~g << "\n";
    g ^ 'A';
    cout << g.GetIndex(1,32)<< g.GetIndex(1,30) << "\n";
}

/* 
testcase14
                                                                                                                                                                                                                                                                                       Z
31
A
*/

void testcase15() {
      DynamicGrid<double> g;
  
      g(2,5) = 42.5;
      g(0,0) = -1.25;
      cout << g << ~g << "\n";
  
      g ^ 7.0;
      cout << g << ~g << "\n";
  
      ++g;
      cout << g << ~g << "\n";
  
      g.SetIndex(1,1,111.111);
      g.SetIndex(1,22,222.222); // must be ignored (no auto-grow in SetIndex)
      cout << g << ~g << "\n";
  
      cout << g.GetIndex(1,1) << " " << g.GetIndex(1,999) << "\n";
  
      DynamicGrid<double> a(g);
      DynamicGrid<double> b; b = g;
      a(0,1) = 888.888;
      b(0,2) = 999.999;
      cout << a << ~a << "\n";
      cout << b << ~b << "\n";
  
      g = g; 
      cout << g << ~g << "\n";
  
      DynamicGrid<double> z;
      z ^ 3.14159;
      cout << z << ~z << "\n";
  
      g(0,100) = 123.456; // trigger expandRow on a far column
      cout << g.GetIndex(0,100) << "\n";
  
      ifstream f("double2.txt"); 
      if (f) {
          DynamicGrid<double> r;
          f >> r;
          f.close();
  
          cout << r << ~r << "\n";
          ++r;              // pad rows to max width found
          r ^ 666.6;        // append a new filled row
          r(10,7) = 5.55;   // grow or access by reference
          cout << r << ~r << "\n";
      }
  }
  
/*
testcase15
     -1.25         0         0         0
         0         0         0         0
         0         0         0         0         0      42.5
14
     -1.25         0         0         0
         0         0         0         0
         0         0         0         0         0      42.5
         7         7         7         7         7         7
20
     -1.25         0         0         0         0         0
         0         0         0         0         0         0
         0         0         0         0         0      42.5
         7         7         7         7         7         7
24
     -1.25         0         0         0         0         0
         0   111.111         0         0         0         0
         0         0         0         0         0      42.5
         7         7         7         7         7         7
24
111.111 0
     -1.25   888.888         0         0         0         0
         0   111.111         0         0         0         0
         0         0         0         0         0      42.5
         7         7         7         7         7         7
24
     -1.25         0   999.999         0         0         0
         0   111.111         0         0         0         0
         0         0         0         0         0      42.5
         7         7         7         7         7         7
24
     -1.25         0         0         0         0         0
         0   111.111         0         0         0         0
         0         0         0         0         0      42.5
         7         7         7         7         7         7
24
   3.14159   3.14159   3.14159   3.14159
4
123.456
     6.929   -92.572    82.037    66.848   -63.005    18.933    90.921   -13.986   -66.127
    -39.25   -51.683    42.449   -40.416         0         0         0         0         0
    98.597   -21.796    58.492   -92.799   -38.953         0         0         0         0
     41.51     44.07    43.454     32.37    -52.87   -16.157   -35.348   -29.483   -65.751
    -72.85   -41.761   -30.501    64.777   -49.823   -19.432         0         0         0
   -46.502   -15.261   -16.319    17.604    98.751         0         0         0         0
    52.811    74.195    51.303    -48.62   -11.493    46.486   -40.149    29.786    71.422
   -89.434   -77.607    78.428    13.608     -94.6    33.805   -72.543         0         0
     -35.1   -24.967   -53.141    76.916    64.019         0         0         0         0
    21.932     3.021     63.86   -16.153     99.87         0         0         0         0
   -50.721   -94.155   -77.446   -16.524    53.114   -27.331    32.214    50.061         0
    89.949     46.18    37.471   -72.011    21.878   -48.755   -94.251         0         0
   -17.828    56.462    51.285   -27.764   -55.004    31.162     22.14     1.265    64.304
   -78.185    44.995    30.166    12.191   -83.398   -67.628    92.802    50.418   -10.308
     53.79   -70.999    55.085     15.47   -49.781         0         0         0         0
   -63.228   -96.238    94.476    28.318    39.979     66.06   -81.591    50.767   -41.211
   -82.291    22.419    19.906      8.45   -80.892     65.04    -2.389    67.708         0
    17.133     37.02     7.263   -24.267         0         0         0         0         0
    -18.79   -67.187   -85.576    70.288    90.207    32.536         0         0         0
    63.661    28.563    -99.07    -47.87   -82.094    89.962    39.821     58.01   -81.009
    25.153      19.8   -15.087    24.811   -20.916   -89.964   -37.972         0         0
   -83.803   -99.801     8.527    -25.53   -58.264    22.151    59.775    79.398   -40.343
   -61.443    15.081    -49.73     26.62   -47.164   -44.337   -56.838         0         0
   -25.393    61.529   -69.311    63.993     -3.49         0         0         0         0
    85.985     -0.27    41.551     2.665         0         0         0         0         0
    -63.71   -45.554   -51.052   -42.099         0         0         0         0         0
    -49.31   -79.217   -10.746   -91.045    88.536   -36.423   -65.353   -30.385         0
   -14.574   -89.865   -18.837   -69.702     0.537    19.447   -55.278         0         0
   -20.735   -84.775    -64.42    27.362   -11.432    -3.897    58.137   -28.283    56.351
   -10.001   -60.973    76.538   -83.927     8.746   -90.035    11.211    63.192         0
270
     6.929   -92.572    82.037    66.848   -63.005    18.933    90.921   -13.986   -66.127
    -39.25   -51.683    42.449   -40.416         0         0         0         0         0
    98.597   -21.796    58.492   -92.799   -38.953         0         0         0         0
     41.51     44.07    43.454     32.37    -52.87   -16.157   -35.348   -29.483   -65.751
    -72.85   -41.761   -30.501    64.777   -49.823   -19.432         0         0         0
   -46.502   -15.261   -16.319    17.604    98.751         0         0         0         0
    52.811    74.195    51.303    -48.62   -11.493    46.486   -40.149    29.786    71.422
   -89.434   -77.607    78.428    13.608     -94.6    33.805   -72.543         0         0
     -35.1   -24.967   -53.141    76.916    64.019         0         0         0         0
    21.932     3.021     63.86   -16.153     99.87         0         0         0         0
   -50.721   -94.155   -77.446   -16.524    53.114   -27.331    32.214      5.55         0
    89.949     46.18    37.471   -72.011    21.878   -48.755   -94.251         0         0
   -17.828    56.462    51.285   -27.764   -55.004    31.162     22.14     1.265    64.304
   -78.185    44.995    30.166    12.191   -83.398   -67.628    92.802    50.418   -10.308
     53.79   -70.999    55.085     15.47   -49.781         0         0         0         0
   -63.228   -96.238    94.476    28.318    39.979     66.06   -81.591    50.767   -41.211
   -82.291    22.419    19.906      8.45   -80.892     65.04    -2.389    67.708         0
    17.133     37.02     7.263   -24.267         0         0         0         0         0
    -18.79   -67.187   -85.576    70.288    90.207    32.536         0         0         0
    63.661    28.563    -99.07    -47.87   -82.094    89.962    39.821     58.01   -81.009
    25.153      19.8   -15.087    24.811   -20.916   -89.964   -37.972         0         0
   -83.803   -99.801     8.527    -25.53   -58.264    22.151    59.775    79.398   -40.343
   -61.443    15.081    -49.73     26.62   -47.164   -44.337   -56.838         0         0
   -25.393    61.529   -69.311    63.993     -3.49         0         0         0         0
    85.985     -0.27    41.551     2.665         0         0         0         0         0
    -63.71   -45.554   -51.052   -42.099         0         0         0         0         0
    -49.31   -79.217   -10.746   -91.045    88.536   -36.423   -65.353   -30.385         0
   -14.574   -89.865   -18.837   -69.702     0.537    19.447   -55.278         0         0
   -20.735   -84.775    -64.42    27.362   -11.432    -3.897    58.137   -28.283    56.351
   -10.001   -60.973    76.538   -83.927     8.746   -90.035    11.211    63.192         0
     666.6     666.6     666.6     666.6     666.6     666.6     666.6     666.6     666.6
279
*/

  
void testcase16() {
      DynamicGrid<string> s;
      s ^ string("Apple");
      s.SetIndex(0,0,"A");
      cout << s << ~s << "\n";
  
      ifstream fs("string1.txt");
      if (fs) {
          DynamicGrid<string> w; fs >> w; fs.close();
          ++w;
          cout << ~w << "\n";
      }
  
      ifstream fc("char2.txt");
      if (fc) {
          DynamicGrid<char> c; fc >> c; fc.close();
          c(0,50) = 'Z';
          cout << c.GetIndex(0,50) << "\n";
      }
  
      ifstream fi("int.txt");
      if (fi) {
          DynamicGrid<int> k; fi >> k; fi.close();
          DynamicGrid<int> t(k);
          DynamicGrid<int> u; u = t; u = u;
          cout << ~u << "\n";
      }
  }
/* 
testcase16
         A     Apple     Apple     Apple
4
25
Z
90
*/


int main() {
    string which;
    cin >> which;
    if (which == "testcase1")  { testcase1();  return 0; }
    if (which == "testcase2")  { testcase2();  return 0; }
    if (which == "testcase3")  { testcase3();  return 0; }
    if (which == "testcase4")  { testcase4();  return 0; }
    if (which == "testcase5")  { testcase5();  return 0; }
    if (which == "testcase6")  { testcase6();  return 0; }
    if (which == "testcase7")  { testcase7();  return 0; }
    if (which == "testcase8")  { testcase8();  return 0; }
    if (which == "testcase9")  { testcase9();  return 0; }
    if (which == "testcase10") { testcase10(); return 0; }
    if (which == "testcase11") { testcase11(); return 0; }
    if (which == "testcase12") { testcase12(); return 0; }
    if (which == "testcase13") { testcase13(); return 0; }
    if (which == "testcase14") { testcase14(); return 0; }
    if (which == "testcase15") { testcase15(); return 0; }
    if (which == "testcase16") { testcase16(); return 0; }

    return 0;
}
