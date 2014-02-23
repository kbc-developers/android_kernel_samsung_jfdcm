ui_print("");
ui_print("");
ui_print("------------------------------------------------");
ui_print("@VERSION");
ui_print("  KBC Developers:");
ui_print("    HomuHomu");
ui_print("    ma34s3");
ui_print("    sakuramilk");
ui_print("------------------------------------------------");
ui_print("");
show_progress(0.500000, 0);

ui_print("flashing boot image...");
package_extract_file("boot.img", "/tmp/boot.img");
package_extract_dir("loki", "/tmp/loki");
set_perm(0, 0, 0755, "/tmp/loki/loki.sh");
set_perm(0, 0, 0755, "/tmp/loki/loki_flash");
set_perm(0, 0, 0755, "/tmp/loki/loki_patch");

assert(run_program("/tmp/loki/loki.sh") == 0);
delete("/tmp/loki");

show_progress(0.100000, 0);

ui_print("flash complete. Enjoy!");
set_progress(1.000000);

