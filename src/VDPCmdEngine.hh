// $Id$

#ifndef __VDPCMDENGINE_HH__
#define __VDPCMDENGINE_HH__

#include "openmsx.hh"
#include "EmuTime.hh"

class VDP;

/** VDP command engine by Alex Wulms.
  * Implements command execution unit of V9938/58.
  */
class VDPCmdEngine
{
public:
	/** Constructor.
	  */
	VDPCmdEngine(VDP *vdp, const EmuTime &time);

	/** Gets the command engine status (part of S#2).
	  * Bit 7 (TR) is set when the command engine is ready for
	  * a pixel transfer.
	  * Bit 4 (BD) is set when the boundary colour is detected.
	  * Bit 0 (CE) is set when a command is in progress.
	  */
	inline byte getStatus(const EmuTime &time) {
		return status;
	}

	/** Use this function to transfer pixel(s) from VDP to CPU.
	  * @param time The moment in emulated time this read occurs.
	  * @return Colour value of the pixel.
	  */
	byte read(const EmuTime &time);

	/** Synchronises the command engine with the VDP.
	  * @param time The moment in emulated time to sync to.
	  */
	inline void sync(const EmuTime &time) {
	  // TODO: Currently, commands are executed instantaneously.
	}

	/** Gets the X coordinate of a border detected by SRCH.
	  * @param time The moment in emulated time this get occurs.
	  */
	inline int getBorderX(const EmuTime &time) {
		return borderX;
	}

	/** Writes to a command register.
	  * @param index The register [0..14] to write to.
	  * @param index The new value for the specified register.
	  * @param time The moment in emulated time this write occurs.
	  */
	inline void setCmdReg(byte index, byte value, const EmuTime &time) {
		// TODO: fMSX sets the register after calling write,
		//       with write setting the register as well.
		//       Is there a difference? Which is correct?
		cmdReg[index] = value;
		if (index == 12) write(value, time);
		else if (index == 14) executeCommand();
	}

	/** Informs the command engine of a VDP display mode change.
	  * @param mode The new display mode: M5..M1.
	  * @param time The moment in emulated time this change occurs.
	  */
	void updateDisplayMode(int mode, const EmuTime &time);

	/** Informs the command engine of a change in VRAM contents.
	  * TODO: Maybe this is a performance problem, if so think of a
	  *   smarter way to update (for example, subscribe to VRAM
	  *   address regions).
	  * @param addr The address that will change.
	  * @param time The moment in emulated time this change occurs.
	  */
	inline void updateVRAM(int addr, const EmuTime &time) {
		// TODO: Sync until time if necessary.
	}

private:

	// Types:

	typedef void (VDPCmdEngine::*EngineMethod)();

	// Methods:

	/** Use this function to transfer pixel(s) from CPU to VDP.
	  * @param value Colour value of the pixel.
	  * @param time The moment in emulated time this write occurs.
	  */
	void write(byte value, const EmuTime &time);

	/** Perform a number of steps of the active operation.
	  * TODO: Legacy method, should be replaced eventually.
	  */
	void loop();

	/** Calculate addr of a pixel in VRAM.
	  */
	inline int vramAddr(int x, int y);

	/** Get a pixel on SCREEN5.
	  */
	inline byte point5(int sx, int sy);

	/** Get a pixel on SCREEN6.
	  */
	inline byte point6(int sx, int sy);

	/** Get a pixel on SCREEN7.
	  */
	inline byte point7(int sx, int sy);

	/** Get a pixel on SCREEN8.
	  */
	inline byte point8(int sx, int sy);

	/** Get a pixel on the screen.
	  */
	inline byte point(int sx, int sy);

	/** Low level function to set a pixel on a screen.
	  */
	inline void psetLowLevel(int addr, byte cl, byte m, byte op);

	/** Set a pixel on SCREEN5.
	  */
	inline void pset5(int dx, int dy, byte cl, byte op);

	/** Set a pixel on SCREEN6.
	  */
	inline void pset6(int dx, int dy, byte cl, byte op);

	/** Set a pixel on SCREEN7.
	  */
	inline void pset7(int dx, int dy, byte cl, byte op);

	/** Set a pixel on SCREEN8.
	  */
	inline void pset8(int dx, int dy, byte cl, byte op);

	/** Set a pixel on the screen.
	  */
	inline void pset(int dx, int dy, byte cl, byte op);

	/** Perform a given V9938 graphical operation.
	  */
	void executeCommand();

	/** Get timing value for a certain VDP command.
	  * @param timingValues Pointer to a table containing the timing
	  *   values for the VDP command in question.
	  */
	int getVdpTimingValue(const int *timingValues);

	// Engine functions which implement the different commands.

	/** Do nothing.
	  */
	void dummyEngine();

	/** Search a dot.
	  */
	void srchEngine();

	/** Draw a line.
	  */
	void lineEngine();

	/** Logical move VDP -> VRAM.
	  */
	void lmmvEngine();

	/** Logical move VRAM -> VRAM.
	  */
	void lmmmEngine();

	/** Logical move VRAM -> CPU.
	  */
	void lmcmEngine();

	/** Logical move CPU -> VRAM.
	  */
	void lmmcEngine();

	/** High-speed move VDP -> VRAM.
	  */
	void hmmvEngine();

	/** High-speed move VRAM -> VRAM.
	  */
	void hmmmEngine();

	/** High-speed move VRAM -> VRAM (Y direction only).
	  */
	void ymmmEngine();

	/** High-speed move CPU -> VRAM.
	  */
	void hmmcEngine();

	/** Report to stdout the VDP command specified in the registers.
	  */
	void reportVdpCommand();

	// Fields:

	struct {
		int SX,SY;
		int DX,DY;
		int TX,TY;
		int NX,NY;
		int MX;
		int ASX,ADX,ANX;
		byte CL;
		byte LO;
		byte CM;
	} MMC;

	/** VDP command registers.
	  */
	byte cmdReg[15];

	/** The command engine status (part of S#2).
	  * Bit 7 (TR) is set when the command engine is ready for
	  * a pixel transfer.
	  * Bit 4 (BD) is set when the boundary colour is detected.
	  * Bit 0 (CE) is set when a command is in progress.
	  */
	byte status;

	/** The X coordinate of a border detected by SRCH.
	  */
	int borderX;

	/** Operation timing.
	  */
	int opsCount;

	/** Pointer to engine method that performs current command.
	  */
	EngineMethod currEngine;

	/** The VDP this command engine is part of.
	  */
	VDP *vdp;

	/** Current screen mode.
	  * 0 -> SCREEN5, 1 -> SCREEN6, 2 -> SCREEN7, 3 -> SCREEN8,
	  * -1 -> other.
	  */
	int scrMode;

	/** Current time: the moment up until when the engine is emulated.
	  */
	EmuTimeFreq<21477270> currentTime;

};

#endif // __VDPCMDENGINE_HH__
