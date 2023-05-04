for i in ./Tests/*.cmm
do 
    ./Code/parser $i $i.ir;
    if [ $? -ne 0 ]; 
    then
        echo $i;
    fi
done