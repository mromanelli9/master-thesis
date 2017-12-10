# Patch
Collection of patches for:
* _NetAnim_: This patch will remove the limitation for old version of netanim xml files.
* _ns-3_:
	* Obstacle Model: original Obstacle Shadowing Model
	* NS2 Mobility Model: modification of Ns2MobilityHelper class in which the model
	can read only the initial positions (fixed mobility)
	* 3D Obstacle Model: 3D extension of Obstacle Shadowing Model


### Prerequisites
NetAnim 3.107, ns-3.23, git

### Installation
Type `git apply $filename.diff` inside software main folder.
