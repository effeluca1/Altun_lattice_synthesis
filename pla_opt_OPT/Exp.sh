#! /bin/bash
for  pla in $(ls *.pla | grep -v optim);
do
    string='lattice -u -OptCol';
    total=$pla$string;
    echo ./AltunSinthesis $pla $total;
#    ./AltunSinthesis $pla $total
done
