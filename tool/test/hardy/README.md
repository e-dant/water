# Hardy

## usage

```
  hardy.sh <-s,--start [n] | -d,--die>
```

## summary

```
  hardy is a utility that spawns and
  destroys difficult processes.

  internally, it reads from 
  '/dev/urandom', forever, making 
  endless 'shasum's of the endless, 
  random input.

  luckily, hardy can die.
  this has the effect of saving your
  computer from sounding like it's 
  entering the atmosphere at low-
  earth orbit.
```

## examples

```
  # start 5 difficult processes
  hardy.sh --start 5

  # stop being difficult
  # (kill all the spawned processes)
  hardy.sh --die

  # start and stop 100 instances
  hardy.sh -s 100;hardy.sh -d
```

