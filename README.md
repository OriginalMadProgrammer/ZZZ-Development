# ZZZ-Development
Stuff still under development but far enough along to be worth sharing. 

## isbare.sh
Check if Linux host is "bare metal". Returns success if so. 
Else returns code providing "best guess" at VM type.


## rc.user
Powerful login script. While more Baroque than typical users will want,
I've had to work in some wildly complex environments where this makes sense.

This was written a long time ago and worked well for me then. However, over time it's drifted and needs much work. Work is slowely being done.

Likely the best way to pull this down is to first fork the repository.

```bash 
cd
mkdir gilbertshub
cd gilbertshub
git clone http://github.com/GilbertsHub/ZZZ-Development
cd ~
ln -s gilberthub/ZZZ-Development/rc.user
```
