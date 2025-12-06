# 160-Core RV32EC M.2 Supercluster

![M.2 2230 cards full of chips mounted vertically on a SSD-sized M.2 add-in card installed inside a partially disassembled mini PC with screen in background](photos/supercluster_view1.png)

---

This supercluster consists of 10 vertically-mounted M.2 2230 modules containing 16 CH32V203C8T6 microcontrollers (not compatible with real M.2 slots) on an M.2 2280 baseboard. Host-to-cluster communication goes through a PCI Express UART controller connected to a CH32V208 on the bottom of the baseboard. Communications inside the cluster go through a custom bus that connects all 160 CH32V203s nodes and the CH32V208 baseboard controller together. Full explanation is in the video below.

<iframe src="https://www.youtube.com/embed/HRfbQJ6FdF0"></iframe>

## Photos

![One M.2 2230 module containing 16 CH32V203C8T6 microcontrollers with supporting circuitry](photos/supercluster_module.png)

![](photos/module_programmer_with_live_module.png)

![](photos/supercluster_on_ngff_to_pcie_adapter.png)
