#! /bin/bash

cd ..

cmd0=(menu.c quit.c)

cmd1=(ls.c cd.c pwd.c mkdir.c rmdir.c creat.c link.c unlink.c symlink.c rm.c chmod.c chown.c stat.c touch.c)

cmd2=(open.c close.c pfd.c lseek.c access.c read.c write.c cat.c cp.c mv.c)

cmd3=(mount.c umount.c cs.c sync.c)


for file in "${cmd0[@]}"
do
    echo "cmd/$file"
    touch "cmd/$file"
done

for file in "${cmd1[@]}" 
do
    echo "cmd/level_1/$file"
    touch "cmd/level_1/$file"
done

for file in "${cmd2[@]}" 
do
    echo "cmd/level_2/$file"
    touch "cmd/level_2/$file"
done

for file in "${cmd3[@]}" 
do
    echo "cmd/level_3/$file"
    touch "cmd/level_3/$file"
done
