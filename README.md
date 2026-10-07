# Worksheet 5 hands-on: anonymous pipes

The programs for the hands-on part of the Week 5 worksheet
(<https://os26.sys-nthu.tw/weeks/w5.html#hands-on>), ready to build.
You still have to run each step yourself and think about what you see.

## Quick start

Open this repository in a GitHub Codespace
(<https://codespaces.new/sys-nthu/os26-w5>); it runs `setup.sh` and `make`
for you when it is created. On any other Linux machine, clone it and run:

```sh
./setup.sh   # once: checks gcc, make, strace, pstree, ps, python3, git, timeout and installs what is missing
make         # builds 01-pipeline, 02-numbers, 02-sum, 04-capacity, 04-worker, 04-capture*
```

`setup.sh` uses `apt-get` (with `sudo` if you are not root). On another
distribution, install the same tools by hand and run `make`.

Several programs hang on purpose. Each one prints a hint to stderr right
before the call that blocks; press Ctrl-C, or run it as
`timeout 3 ./program` and expect exit status 124.

## What each step uses

Each file name starts with the number of the step in the hands-on part that uses it.

1. **Build a pipeline by hand.** `01-pipeline.c` does what the shell does for
   `cat scores.csv | grep niko`: `pipe`, `fork`, `dup2`, `execlp`, twice.
   Run `./01-pipeline` and expect `niko,95`, then
   `strace -f -e trace=pipe2,dup2,close,execve,clone,clone3,wait4 ./01-pipeline`
   and match each line to a line of the source. (Failed `execve` lines ending
   in `ENOENT` are `execlp` trying each `PATH` directory in turn; ignore them.)
2. **Look at a live pipe.** No program to build. Run `pstree -ps $$` to see your
   shell's ancestors. Then run `sleep 1000 | wc -l &` and `ls -l /proc/<pid>/fd`
   for both processes to see
   the same `pipe:[inode]` on `sleep`'s fd 1 and `wc`'s fd 0.
3. **stdout is data, stderr is talk.** `02-numbers.c` prints `1..N`,
   `02-sum.c` adds up what it reads. `./02-numbers 5 | ./02-sum` gives
   `sum = 15`; `DEBUG=1 ./02-numbers 5 | ./02-sum` fails because the
   debugging message went down the pipe. Swap the two marked lines in
   `02-numbers.c`, rebuild, and run it again.
4. **Capturing a child's output, two bugs.** `04-capture.c` reads to EOF first
   and then waits. `04-capture-noclose.c` differs by one line (the parent's
   `close(fd[1])` is missing): `timeout 3 ./04-capture-noclose 1000` hangs after
   the worker exits, and `ls -l /proc/<parent pid>/fd` shows fd 4 still on the
   pipe. `04-capacity.c` prints the pipe capacity.
   `04-worker.c` writes N bytes. `04-capture-broken.c` calls `wait()` before
   reading: `./04-capture-broken 1000` works, `./04-capture-broken 1000000`
   hangs. While it hangs, `ps -o pid,stat,wchan:20,cmd` on both processes shows
   where each one sleeps in the kernel. `04-capture.c` reads to EOF first and
   then waits. `04-capture.py` reproduces both versions with Python's
   `subprocess` module.
5. **A pipe you use every day.** No program to build. In any git repository
   run `strace -f -e trace=pipe2,dup2,execve git log` and watch git create a
   pipe and `exec` your pager on the other end.
6. **Three signals, three exit statuses.** No program to build. Start
   `sleep 1000 &` three times and stop it with `kill -INT %1`, `kill -TERM %1`
   and `kill -KILL %1`, reading the status with `wait %1; echo $?` each time
   (130, 143, 137). Then press Ctrl-C on `sleep 1000 | cat`.
7. **timeout sends a signal when the time is up.** No program to build. Compare
   `timeout -v 2 sleep 10` (status 124, after SIGTERM), `timeout -s KILL 2 sleep 10`
   (137) and `timeout -k 2 -v 2 sleep 10`.
8. **Optional: SIGPIPE upstream, end of file downstream.** No program to
    build. Run `yes | cat | wc -l`, kill `cat` from a second terminal with
    `pkill -x cat`, then `echo "${PIPESTATUS[@]}"` shows `141 143 0`.

## Notes

- Pipe data is always moved with `write()` and `read()`, never `printf`, so
  stdio buffering cannot confuse what you see in `strace`.
- `make clean` removes the binaries. Core dumps, if your shell writes them,
  are ignored by git.
