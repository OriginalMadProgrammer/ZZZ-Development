# ZZZ-Development
Stuff still under development but far enough along to be worth sharing. 

## isbare.sh
Check if Linux host is "bare metal". Returns success if so. 
Else returns code providing "best guess" at VM type.

Status: beta testing


## rc.user
Powerful login script library. While more Baroque than typical users will
need, or even want,
I've had to work in some wildly complex environments where this made sense.
Environments where “heterogeneous” is a mild term for the diversity of
hardware, OSs, and associated versions, being used at once.
Think automouting of home directories. 
Then there is the need for so many custom programs in different environments
clashing with the desire to keep a common source tree for all.

Status: redevelopment

rc.user was written a long time ago and worked well for me then.
However, over time it's drifted and needs much work. Work is slowly being done.

Likely the best way to pull this down is to first fork the repository
then run the following commands:

```bash 
  cd
  mkdir -p src/gilbertshub
  cd src/gilbertshub
  git clone http://github.com/$YourGithubAccount/ZZZ-Development
  cd ~
  ln -s src/gilberthub/ZZZ-Development/rc.user
```

Then follow the make/install instructions from within $HOME/rc.user.
