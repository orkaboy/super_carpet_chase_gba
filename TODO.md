# TODO list for Super Carpet Chase

[x] Main entrypoint
[ ] Gamestate
    [x] FSM structure for overarching game states (menu/cutscene/game etc.)
    [x] Menu
        [ ] Fade to black
        [ ] Add "Super"
        [ ] Add "Press Start"
    [ ] Intro
        [ ] Text engine/font (use tte? or just a bitmap font?)
        [ ] Graphics
            [ ] BG, wizards
            [ ] Special effects (palette swap)
        [ ] Script/storyboard
    [ ] Game
    [ ] Game Over
    [ ] Outro
[ ] Mode7 background
    [x] Tech
    [x] Camera movement
        [ ] Figure out the resolution on theta/phi
    [x] The background + tile-map
    [x] Skybox
        [x] Scroll effect
        [ ] Fix the sky palette being wrong (shared with desert bg)
[ ] Graphics
    [ ] Player sprite + carpet
    [ ] Boss sprite + carpet
    [ ] Player spells
    [ ] Boss spells
    [ ] Summons
    [ ] Graphics initialization
    [ ] Final mega-phase, some big BG block added for the boss (Mode 0/1 regular BG)
[ ] Game logic
    [ ] Collisions
    [ ] Movement updates
    [ ] I-frames
    [ ] Fire spells
[ ] Audio
    [x] Systems and temp music in place
    [ ] Music
    [ ] SFX
[ ] Effects
    [ ] Hit effect (palette swap?)
    [ ] Death effect (thrown off carpet, animation + SFX + slowdown?)
[ ] GUI layer
    [ ] Player hp
    [ ] Boss hp
[ ] Power ups
    [ ] Health
    [ ] New spells (scrolls?)
    [ ] Shield
[ ] Story
    [ ] Cut-scene engine
    [ ] Close-up wizard art
    [ ] Text
    [ ] Fade to black effects
[ ] Boss patterns and logic
    [ ] AI core
    [ ] Adaptive patterns
    [ ] Use shield for bullet-hell phase
