# DeskMaster

DeskMaster is a custom component for ESPHome to control an UpLift (or similar) motorized standing desk

## Background

This project is a fork of the [desky](https://github.com/ssieb/custom_components/tree/master/components/desky) component by [@ssieb](https://github.com/ssieb).
I wanted to add extra features and scope creep well outside of that of of Desky so I created this (fork) project

# Connections

Below are the connections of the desk controller with both the color code of the cable attached to the display as well as the color code of standard ethernet (T-568B)

| display color | ethernet color | single wire function           | Preset 1     | Preset 2     | Preset 3     | Preset 4     |
|---------------|----------------|--------------------------------|--------------|--------------|--------------|--------------|
| transparent   | orange stripe  | M button (when LOW)            |              |              |              |              |
| brown         | orange         | digital data                   |              |              |              |              |
| black         | green stripe   | GND                            |              |              |              |              |
| white         | blue           | request height data (when LOW) |              |              |              |              |
| red           | blue stripe    | +5v                            |              |              |              |              |
| purple        | green          | preset 2 (when LOW)            |              | 2 (when LOW) | 3 (when LOW) | 4 (when LOW) |
| green         | brown stripe   | UP (when LOW)                  | 1 (when LOW) |              |              | 4 (when LOW) |
| yellow        | brown          | DOWN (when LOW)                | 1 (when LOW) |              | 3 (when LOW) |              |


# References

These resources were invaluable to the creation and development of this project

* <https://hackaday.io/project/4173-uplift-desk-wifi-link> ([wayback](https://web.archive.org/web/2/https://hackaday.io/project/4173-uplift-desk-wifi-link))
* <https://community.home-assistant.io/t/desky-standing-desk-esphome-works-with-desky-uplift-jiecang-assmann-others/383790/18> ([wayback](https://web.archive.org/web/2/https://community.home-assistant.io/t/desky-standing-desk-esphome-works-with-desky-uplift-jiecang-assmann-others/383790/18))
