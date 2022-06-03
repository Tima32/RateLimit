# RateLimit
## Description
Program for configuring traffic speed limits DEVICE_NAME.
## Build
```sh
make all
```
## Usage
Usage: rate_limit [command] {args}
- Commands:
- enable  [port number] Enables traffic limiting.
- desable [port number] Disables traffic limiting.
- stat    [port number] Displays the port setting.
- rate    [port number] [rate] Set rate limit.

Rate is specified in bit\s. If rate is not a multiple of 250,000, then it is rounded up to a lower multiple.

## Example
```sh
rate_limit enable 0
```

## Author
Golovlev Timofey 4timonomit4@gmail.com
