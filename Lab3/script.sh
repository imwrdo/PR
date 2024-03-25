#!/bin/bash

declare -a commands=()

declare -a pids=()

add_command() {
    commands+=("$@")
}


add_pid() {
    pids+=("$1")
}


wait_for_processes() {
    for pid in "${pids[@]}"; do
        wait "$pid"
    done
}


run_all_commands() {
    for cmd in "${commands[@]}"; do
        eval "$cmd" &
        add_pid $!
    done
}


display_menu() {
    echo "Menu:"
    echo "1. Dodaj własne polecenie"
    echo "2. Uruchom wszystkie polecenia"
    echo "3. Wyjdź"
}

add_command "wget -O file.txt https://pl.wikipedia.org/wiki/Wikipedia:Strona_g%C5%82%C3%B3wna"
add_command "ping -c 10 pg.edu.pl"
add_command "sleep 10"
add_command "uname -a"
add_command "ps aux"

while true; do
    display_menu

    read -p "Wybierz opcję: " choice

    case $choice in
        1)  read -p "Podaj nowe polecenie: " command
            add_command "$command" ;;
        2)  if [ ${#commands[@]} -eq 0 ]; then
                echo "Nie dodano jeszcze żadnych poleceń."
            else
                echo "Uruchamianie wszystkich poleceń..."
                run_all_commands
                wait_for_processes
                echo "Wszystkie procesy zakończone."
                exit
            fi
            ;;
        3)  exit ;;
        *)  echo "Nieprawidłowy wybór. Wybierz opcję od 1 do 3." ;;
    esac
done
