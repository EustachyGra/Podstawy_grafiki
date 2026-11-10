https://chatgpt.com/share/6ad650c4-5fa4-83ed-8005-49cb4ad22aac
https://oneuptime.com/blog/post/2026-03-20-rip-linux-frrouting/view
https://chatgpt.com/share/6ae000fb-b3f8-83ed-8fb1-020e583df7db
```
https://docs.strongswan.org/docs/latest/config/quickstart.html
https://docs.strongswan.org/docs/latest/howtos/introduction.html
https://docs.strongswan.org/docs/latest/swanctl/swanctlConf.html
```
```
connections {

    lab {

        version = 2

        local_addrs = 192.168.70.1
        remote_addrs = 192.168.70.2

        proposals = aes256-sha256-modp2048

        local {
            auth = psk
            id = 192.168.70.1
        }

        remote {
            auth = psk
            id = 192.168.70.2
        }

        children {

            lan {

                local_ts = 10.10.70.0/30
                remote_ts = 10.10.70.4/30

                esp_proposals = aes256-sha256

                start_action = trap
                dpd_action = restart

            }
        }
    }
}

secrets {

    ike-lab {

        id-1 = 192.168.70.1
        id-2 = 192.168.70.2

        secret = "TWOJ_KLUCZ_PSK"

    }
}
```
```
printf '\033[0m'
vim ~/.bashrc
# Automatyczna animacja kolorów terminala
if [[ $- == *i* ]] && [[ -z "$RAINBOW_RUNNING" ]]; then
    export RAINBOW_RUNNING=1

    (
        while true; do
            FG=$((RANDOM % 256))
            BG=$((RANDOM % 256))

            printf '\033[38;5;%sm' "$FG"
            printf '\033[48;5;%sm' "$BG"

            sleep 2
        done
    ) &
fi

```