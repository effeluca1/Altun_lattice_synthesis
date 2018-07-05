#! /bin/bash
for  pla in *.pla;
do
    string='lattice -u -OptCol';
    total=$pla$string;
    echo ./AltunSinthesis $pla $total;
    ./AltunSinthesis $pla $total
done
