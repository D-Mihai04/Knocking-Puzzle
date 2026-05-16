# Knocking Detection Puzzle (Vibration Sensor + Arduino Nano)

## Overview
This project is a simple interactive puzzle using a vibration/knock sensor attached to a wooden surface (door/plank). The system detects knock patterns and triggers an action when the correct sequence is entered.

The target sequence can be modified manually, it currently is:
**2 knocks → 3 knocks → 1 knock**

---

## Hardware Requirements
- Arduino Nano
- Vibration / Knock sensor module
- Breadboard
- Wooden plank or door (mounting surface)

---

## How It Works

This project uses a vibration sensor attached to a wooden surface to detect knock patterns and runs a finite state machine to interpret them.

### Core Idea
The system progresses through states as the user enters knocks:

- WAITING → idle, waiting for first knock  
- GROUP1 → records first sequence (must be 2 knocks)  
- PAUSING → short gap between sequences  
- GROUP2 → records second sequence (must be 3 knocks)  
- PAUSING2 → short gap  
- GROUP3 → records final sequence (must be 1 knock)  
- SUCCESS → correct code entered

At any point, invalid input or timeout resets the system back to WAITING.


### Timing Rules
- Knocks must be spaced within a maximum gap window
- Pauses between groups must be within allowed time limits
- Entire sequence has a global timeout


## Behavior
### Correct Sequence
2 → 3 → 1 knocks in order triggers:
- LED ON / pattern unlock / output activatio


### Wrong Sequence
- Sequence resets automatically
- User must restart pattern

---

## Goal
Create a simple puzzle where users must discover and input the correct knocking pattern to unlock a response.
