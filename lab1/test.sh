for i in ./Tests/*.cmm
do 
    ./Code/parser $i $i.ir;
done