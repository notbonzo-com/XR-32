## TODO in future revisions

1.
### Memory Mapped I/O (MMIO)

Some specific devices like the Advanced Configuration and Power Interface (ACPI) or the PCI bus can be mapped into the XR-32 architecture's memory space. This allows these devices to be accessed using standard memory instructions like `LDR` and `STR`. XR-32 is designed to be compatible with ACPI standard as described in the UEFI Specification. For more information, refer to the [ACPI Specification](https://uefi.org/htmlspecs/ACPI_Spec_6_4_html/Frontmatter/Overview/Overview.html)