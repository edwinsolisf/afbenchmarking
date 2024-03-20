ArrayFire Testing
==========

To run the arrayfire benchmark
```sh
    run_af.sh #backend #device_id
```

where `backend` maybe `cpu`, `opencl`, `cuda`, or `oneapi`, and `device_id` may be the one of the available devices on the specified backend. Leaving it blank uses the default backend and device.

To run the onednn benchmark
```sh
    run_benchdnn.sh #location #engine #device
```
where `location` is the directory where `benchdnn` is located, `engine` is either `cpu` or `gpu`, and `device` is the device id from the list of devices for the selected engine. By default `engine=cpu`, `device=0`.

Depending on the location of `benchdnn` and the onednn libraries, you might need to prefix it with `LD_PRELOAD=$(location of libdnnl)`.