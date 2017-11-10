# s[hell]play

splay is the only :notes: player I need. It is made for me by me and I :heart: it. It is :penguin: only.



## Build splay

To build splay clone the git repo and change into it

```shell
git clone https://github.com/aichingm/splay && cd splay
```

and run one of the four build options:

1. The tiny version`make build` 
2. A version with enabled MPRIS support: `make build-mpris`
3. _A debugging version for the the 1. option `make debug`_
4. _A debugging version for the 2. option `make dbbug-mpris`_

this will create a binary called in the _cwd_ *splay*.

## Colorize splay

To change the colors edit the color definitions in `src/colors.h` and rebuild splay.

## Install splay

Just move the *splay* binary in any of the directories which are in your `$PATH`.

**No don't actually do this!** Move it for example to `/usr/local/bin` or create a `bin` directory in your `~` and add it to your `$PATH` 

In order to run (and build) you need **some** of the below listed libraries on your system:

- libvlc 
- ncurses 
- libtinfo
- dbus (only for build with MPRS support)



## Play all the tunes 

splay reads a list of files from `stdin` and plays one after another.  splay can play all formats vlc can play.

For the purpose of usage I will assume that you have some music in `~/Music` , 'cause that's where music belongs.

#### Basic usage

​Play your :blue_heart: song

```shell
echo best_song_ever.ogg | splay
```

Play all music in the `cwd`

```shell
ls -N1 | splay
```

Play all your music

```shell
find ~/Music | splay
```

Play all your ogg's

```shell
find ~/Music/*.ogg | splay
```



#### Advanced usage

splay can also play in shuffle mode

```shell
find ~/Music | shuf | splay
```

Play your music ordered by name *(add -r after sort to reverse)*

```shell
find ~/Music | sort | splay  
```

Hey DJ, loop it :cd:

```shell
while true; do echo ~/Music/nyan_cat_poor_quality.mp3 | splay; done
```

Play all your music in a loop 

```shell
while true; do find ~/Music | splay; done
```

Play the 10 most recently added tracks

```shell
find ~/Music -type f -printf '%T@ %p\0' | sort -zk 1nr | sed -z 's/^[^ ]* //' | tr '\0' '\n' | head -n 10 | splay
```

**I will stop now, I think you got it :bulb:**

#### splay does playlists too

Create a playlist of all your ogg music 

```shell
find ~/Music*.ogg > ~/Playlists/cool_songs.plist
```

Add a song to a playlist

```shell
echo ~/Music/nyan_cat_poor_quality.mp3 >> ~/Playlists/cool_songs.plist
```

Play a playlist with splay

```shell
cat ~/Playlists/cool_songs.plist | splay
```



#### Hit all the Keys

There are only 11 defined 

* `e` , `end` end splay
* `up` move the selection up
* `down` move the selection not up
* `enter`  play the selected song
* `n` play the next song
* `p` play the previous song
* `space` switch between playing and pause 
* `left` seek ten seconds backwards
* `right` seek ten seconds in to the future
* `t` cycle through title display modes :arrows_counterclockwise:

