rm -f aglnxmenu2.zip

cd firststage
make clean all
cp menu.bin ..

cd ..
make clean all

zip -r aglnxmenu2.zip menu.bin menu2stg.bin README.md _preview menu
