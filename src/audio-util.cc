#if 0
    wav = audio.inv_preemphasis(wav )

def inv_preemphasis(x):
  return scipy.signal.lfilter([1], [1, -hparams.preemphasis], x)


https://github.com/scipy/scipy/blob/master/scipy/signal/lfilter.c.src

https://github.com/vinniefalco/DSPFilters

#endif
