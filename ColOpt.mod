param m;
param n;
param q;

# SETS
set I := 1 .. n; # rows
set J := 1 .. m; # column start
set K := 1 .. q; # value

# DATA
param a{i in I , j in J , k in K};

#VARS
var y{j in J , l in J} binary; 
var z{i in I , j in J , k in K} binary;
var min_zj_zv{i in I, j in J, v in J, k in K} binary ;

#OBJ
#come definisco coppie u,v wrt i.... 
#maximize R: sum{k in K} sum{i in I,j in J,v in J: abs(v-j)=1 } x[i,j,v,k]; # controllare
maximize R: sum{k in K} sum{i in I,j in J,v in J: abs(v-j)=1} min_zj_zv[i,j,v,k];
                                                                                   
#CONSTR
subject to assign_try{i in I , j in J , k in K}: z[i,j,k]= sum{l in J} y[j,l] * a[i,l,k];

subject to consistency_I{l in J}: sum{j in J} y[l,j]=1;

subject to min_zj {i in I, j in J, v in J, k in K}: min_zj_zv[i,j,v,k] <= z[i,j,k];
subject to min_zv {i in I, j in J, v in J, k in K}: min_zj_zv[i,j,v,k] <= z[i,v,k];

subject to consistency_L{j in J}: sum{l in J} y[l,j]=1; 
#subject to mapping {i in I, j in J, v in J, k in K}: x[i,j,v,k] <=  z[i, j, k] + z[i, v, k] - 1 ;#controllare

solve;
#display I;
#display J;
#display a;
#display K;
#display z;
display y;
#display x;
#display R;
end;
