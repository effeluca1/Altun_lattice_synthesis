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
var y{i in I , l in I} binary; 
var z{i in I , j in J , k in K} binary;
var min_zj_zv{i in I, j in J, v in I, k in K} binary ;

#OBJ

maximize R: sum{k in K} sum{i in I,j in J,v in I: abs(v-i)=1} min_zj_zv[i,j,v,k];
                                                                                   
#CONSTR
subject to assign_try{i in I , j in J , k in K}: z[i,j,k]= sum{l in I} y[i,l] * a[l,j,k];

subject to consistency_I{l in I}: sum{i in I} y[i,l]=1;
subject to consistency_L{i in I}: sum{l in I} y[i,l]=1; 


subject to min_zj {i in I, j in J, v in I, k in K}: min_zj_zv[i,j,v,k] <= z[i,j,k];
subject to min_zv {i in I, j in J, v in I, k in K}: min_zj_zv[i,j,v,k] <= z[v,j,k];

solve;
#display I;
#display J;
#display a;
#display K;
#display z;
display y;
#display x;
#display R;


#printf '#################################\n';
#printf 'Transportation Problem / LP Model Result\n';
#printf '\n';
#printf 'Minimum Cost = %.2f\n', R;
#
#
#printf '\n';
#printf '\n';
# #printf{i in I, j in J,v in I: abs(v-j)=1}:'%1s %1s %1s\n',i,j,v;
#
#printf '\n';
#printf '\n';
#
#printf{j in J}:'jRz %1s %5s\n',j, sum{i in I,k in K,v in I: abs(v-j)=1 } min((z[i, j, k] + z[i, v, k]));
#printf{j in J}:'jRa %1s %5s\n',j, sum{i in I,k in K,v in I: abs(v-j)=1 } min((a[i, j, k] + a[i, v, k]));
#
#printf '\n';
#printf '\n';
#
#
##printf{k in K, i in I, j in J,v in I: abs(v-j)=1}:'zR %1s %1s %1s %1s %3s\n',k,i,j,v,  z[i, j, k] + z[i, v, k] - 1;
#
#
##printf{ i in I, j in J,v in I: abs(v-j)=1}:'z4jv %1s %1s %1s %1s %3s %3s\n',4,i,j,v,  z[i, j, 4],   z[i, v, 4];
#
##printf{ i in I, j in J,v in I: abs(v-j)=1}:'a4jv %1s %1s %1s %1s %3s %3s\n',4,i,j,v,  a[i, j, 4],   a[i, v, 4];
#
end;
#
