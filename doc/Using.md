### Saving and loading

* FSM diagrams can be saved to (resp. read from) files with the ![](./imgs/save.png)
  (resp. ![](./imgs/open.png)) button of the toolbar (or the by invoking the `Save`, `Save As` or
  `Open` actions in the `File` menu)

* The ![](./imgs/new.png) button (`New` action in the `File` menu) starts the edition of new diagram
  (clearing the current one)

### Editing 

* To give a **name** to a new diagram, use the text field at the top of the left panel

* To **add an input, output or local variable**, click the `Add` button in the relevant box of the
  left panel. This will add a row in which you'll be able to specify its name, type, 
  and, for inputs, the stimuli attached to it (selecting an item in the `stim` selector will bring
  out a dedicated dialog)
  Added inputs, outputs or variables can de deleted by clicking the ![](./imgs/delete.png) at the
  end of the row.

* To **add a state**, select the ![](./imgs/state.png) button in the toolbar and click on the
  canvas. A pop-up dialog gives the opportunity to set the name of the added state and,
  possibly, to attach output valuations. Click `Done` when finished.

* To **add a transition**, select the ![](./imgs/transition.png) button, click on
  the start state and, keeping the mouse button pressed, go the end state and release mouse button.
  A popo-up dialog gives the opportunity to document the transition, by specifiying the triggering
  event and the associated guards and actions. Note that adding a transition requires that at least
  one input with type `event` has been attached to the model. As for states, click `Done` when
  finished. 

* To **add a self transition** (from a state to itself) , select the ![](./imgs/loop.png) button
  and click on start state (the location of the click will decide on that of the transition).

* To **add an initial transition**, select the ![](./imgs/initstate.png) button, click near the
  initial state and, keeping the mouse button pressed, go the  initial state and release mouse
  button. A pop-up dialog gives the opportunity to add actions to the initial transition.
  Click `Done` when finished.

* To **delete a state or a transition**, select the ![](./imgs/delete.png) button
  and click on the state or transition (deleting a state will also delete all incoming and
  outcoming transitions)

* To **move a state**, select the ![](./imgs/select.png) button and drag the state.

* To **edit a state or a transition**, select the ![](./imgs/select.png) button, and right-click (or
  Ctl-Click on a Mac) on the corresponding item 

### Compiling

* To **generate a DOT representation of the  diagram**, click the
  ![](./imgs/compileDOT.png) button (or invoke the corresponding action in the
  `Build` menu)
  
* To **generate `CTask` code **, click the ![](./imgs/compileCTask.png) button

* To **generate `SystemC` code **, click the ![](./imgs/compileSystemc.png) button

* To **generate `VHDL` code **, click the ![](./imgs/compileVHDL.png) button

The generated graphs and code will appear as separate tabs in the right part of the window.
  
### Simulating

To **simulate the diagram** (provided that stimuli have been attached to inputs using the `IOs and
  variables`  in the left part of the window), click the ![](./imgs/runSimulation.png) button (or invoke the
  corresponding action in the `Build` menu)
  
If a valid VCD viewer (such as `gtkwave`) has been specified, simulation results will be displayed
in a separate window.

