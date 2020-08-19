#pragma once

namespace LightControl {
	enum class LightMode {
		OFF,
		ON,
		STROBE,
		GLITTER,
		FREQUENCY_PANEL,
		TETRIS,
		GAME_OF_LIFE,
		RUNNING_RING,
		PULSE,
		RUNNING_SECTIONS,
		RAINBOW,
		FREQUENCY_STRING,
		FREQUENCY_RING,
		FREQUENCY_SINGLE_COLOR,
		FREQUENCY_SINGLE_COLOR_BEAT,
		AUDIO_PULSE, //fade the color on and off. Change the fade speed depending on volume change. Make it possible to activate color change
		AUDIO_GLITTER, //Controll glitter speed and on percent depening on volume or frequencies.
		MATRIX, //Make a matrix like look for panels with falling blocks.
		FREQUENCY_PANEL_FALLING, //Falling blocks with new blocks created depending on frequency power
		FREQUENCY_PANEL_BOUNCE, //Create a block for each frequency group and make it bounce. Bounce speed depends on power level of bin
		FREQUENCY_GLITTER_COLOR, //Create random glitter points. Number of points per color depends on frequency power
		FIRE, //Randomly create points of fire. Make the fire fade away from ignition point.
		FIRE_AUDIO, //Randomly create points of fire. Make the fire fade away from ignition point. Ignition points are spawned by frequency power.
		ENERGY, //Expand from center (off strip, panel or ring). Added colors depends on power and frequency.
		SHOOTING, //Shoot points along the led strip, with fading trails.
		MANDELBROT,
		CNT,
	};
}