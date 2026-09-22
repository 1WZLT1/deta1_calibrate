#打包需要在终端输入以下命令
#python -m PyInstaller --clean --noconfirm --onefile --console --name devicetree_generator --collect-all devicetree device_tree_py.py 
from devicetree import dtlib
from pathlib import Path
from device_tree_generator_deta1   import generator_deta1
from device_tree_generator_deta10  import generator_deta10
from device_tree_generator_deta20  import generator_deta20
from device_tree_generator_deta40  import generator_deta40
from device_tree_generator_deta100 import generator_deta100

import sys

if len(sys.argv) < 4:
    print("错误:请传入 DTS 文件名")
    sys.exit(2)
dts_file = sys.argv[1]
GNSS_Version = sys.argv[2]
Bro_Version = sys.argv[3]
dt = dtlib.DT(dts_file)


if getattr(sys, "frozen", False):
    output_dir = Path(sys.executable).resolve().parent
else:
    output_dir = Path(__file__).resolve().parent

def GNSS_Version_is_enabled(version):
    if version is None:
        return 0
    return 1 if version.upper() == "N" else 0


def Bro_Version_is_enabled(version):
    if version is None:
        return 0
    return 1 if version.lower() == "bro" else 0

def node_is_enabled(node):
    status = node.props.get("status")

    if status is None:
        return 0

    return 1 if status.to_string() == "enable" else 0


dts_name = Path(dts_file).stem.lower()
if dts_name == "deta1":
    header_content = generator_deta1(dt,dts_file,GNSS_Version_is_enabled(GNSS_Version),0,node_is_enabled)
elif dts_name == "deta10":
    header_content = generator_deta10(dt,dts_file,GNSS_Version_is_enabled(GNSS_Version),Bro_Version_is_enabled(Bro_Version),node_is_enabled)
elif dts_name == "deta20":
    header_content = generator_deta20(dt,dts_file,GNSS_Version_is_enabled(GNSS_Version),Bro_Version_is_enabled(Bro_Version),node_is_enabled)
elif dts_name == "deta40":
    header_content = generator_deta40(dt,dts_file,GNSS_Version_is_enabled(GNSS_Version),Bro_Version_is_enabled(Bro_Version),node_is_enabled)
elif dts_name == "deta100":
    header_content = generator_deta100(dt,dts_file,GNSS_Version_is_enabled(GNSS_Version),Bro_Version_is_enabled(Bro_Version),node_is_enabled)

output_file = output_dir / "devicetree_generated.h"
output_file.write_text(header_content, encoding="utf-8")
