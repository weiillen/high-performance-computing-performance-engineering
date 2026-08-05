# MLPerf Llama2-7B Offline benchmark tuning — team project

This team project documents the execution and tuning of an MLPerf Inference v5.1 Llama2-7B Offline, PerformanceOnly run on one AMD GPU. The included report and presentation emphasize validity-first benchmarking: Slurm allocation, Apptainer container construction, bind-mount correctness, vLLM configuration visibility, LoadGen execution, and MLPerf result validation.

## Recorded performance progression

| Stage | Tokens/s |
|---|---:|
| Baseline | 262.591 |
| Higher sequence concurrency | 1,084.520 |
| Best valid run with chunked prefill | 1,193.320 |

The recorded improvement is approximately **4.54×**. The report identifies `max_num_seqs` as the dominant tuning knob in this setup; increasing the token batch limit alone had little effect.

## Engineering blockers documented

- Apptainer construction failed because persistent filesystems were full; temporary build/cache paths were redirected to `/dev/shm`.
- Local YAML changes were initially shadowed by configuration inside the container; a bind mount and an in-container `cat` check established that the intended configuration was actually used.

## Attribution

This is presented as team work. Only the final report and presentation supplied in the archive are included; scripts and job logs whose individual authorship was not established were excluded.

## Original files

- [`original/report.docx`](original/report.docx)
- [`original/presentation.pptx`](original/presentation.pptx)
