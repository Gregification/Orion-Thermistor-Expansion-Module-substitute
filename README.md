# Substitutes for the Orion2 thermistor expansion module
design baselined from Tim Stevens (UTA Racing ~2017) arduino solution (dubbed Timsense board). Timsense code can be found on the UTA Racing file system (originaly made for E17 but its not in the right folder)

## main points
- Cost motivated design. (sub $20)
    - tm4c123gh6 : for quick prototyping since it has a built in CAN handler, all thats really needd is a coupler for the can lines.
        downside is that this chips is ~$12 each (pricy!)
    - msp0l1130x : the ultimate solution (subjective), cheap chip(~$1.5 each),
        and more than fast enough (32Mhz). BUT since it dosent have a internal can pheripherial, we'll need to impliment layer 2 of
        CAN though the MCU, so theres plenty of approaches. we'll also need a can transceiver (<$1).

## after thoughts 
- the CANdaptor (from Orion) is useful in general. it displays can messages transmitted and cna send arbartary messages itself. java swing based so it work on all popular OS's.


### relivent documents
- <a href="https://www.orionbms.com/downloads/misc/thermistor_module_canbus.pdf">orion2 thermistor expansion module can protocol</a>
- also see <a href="https://github.com/Gregification/tempsense_board_e26">my other orion related project</a> for info on the arduion (feather m4) Timsense used.