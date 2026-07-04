#!/usr/bin/env python3
import os
import stat

base_dir = './HW2/extra_python_test'
test_ce_dir = './HW2/test/策問'
test_dian_dir = './HW2/test/殿試'
ce_dir = os.path.join(base_dir, '策問')
dian_dir = os.path.join(base_dir, '殿試')

os.makedirs(ce_dir, exist_ok=True)
os.makedirs(dian_dir, exist_ok=True)

def write_py(path_prefix, py_content):
    with open(f"{path_prefix}.py", "w", encoding="utf-8") as f:
        f.write("#!/usr/bin/env python3\n" + py_content)
    os.chmod(f"{path_prefix}.py", os.stat(f"{path_prefix}.py").st_mode | stat.S_IEXEC)

def write_sh(path):
    sh_content = """#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$DIR/.."
WYC="./build/wyc"
TMPDIR="./extra_python_test/tmp"
mkdir -p "$TMPDIR"

normalize() { sed '1s/^\xef\xbb\xbf//' | tr -d '\r' | sed -e :a -e '/^\\n*$/{$d;N;ba' -e '}' | sed '/./,$!d'; }

ALL_PASSED=true

run_test() {
    local wy_file="$1"
    local expected_file="${wy_file%.wy}.py"
    local filename="$(basename "$wy_file")"
    local exe="$TMPDIR/${filename%.wy}"
    
    "$WYC" "$wy_file" "$exe" 2>/dev/null
    if [ $? -ne 0 ]; then
        echo -e "  FAIL  $wy_file (compile error)"
        ALL_PASSED=false
        return
    fi
    
    actual=$("$exe" 2>&1 | normalize) || true
    expected=$(python3 "$expected_file" 2>&1 | normalize)
    
    if [[ "$actual" == "$expected" ]]; then
        echo "  PASS  $wy_file"
    else
        echo "  FAIL  $wy_file"
        diff <(echo "$expected") <(echo "$actual") | head -10 | sed 's/^/    /'
        ALL_PASSED=false
    fi
}

echo "== Extra Python Test Suite =="
for dir in "策問" "殿試"; do
    if [ -d "extra_python_test/${dir}" ]; then
        for wy_file in "extra_python_test/${dir}"/*.wy; do
            [ -f "$wy_file" ] || continue
            run_test "$wy_file"
        done
    fi
done

if [ "$ALL_PASSED" = true ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed."
    exit 1
fi
"""
    with open(path, "w", encoding="utf-8") as f:
        f.write(sh_content)
    os.chmod(path, os.stat(path).st_mode | stat.S_IEXEC)

def gen_variants(original_file, dest_prefix, replacements_list, py_contents):
    with open(original_file, 'r', encoding='utf-8') as f:
        content = f.read()
    for i, (replacements, py_c) in enumerate(zip(replacements_list, py_contents)):
        v_name = f"{dest_prefix}_v{i+1}"
        new_content = content
        for old_str, new_str in replacements.items():
            new_content = new_content.replace(old_str, new_str)
        with open(f"{v_name}.wy", 'w', encoding='utf-8') as f:
            f.write(new_content)
        write_py(v_name, py_c)

# 00
def py00(val):
    return f"""jia = {val}
print(jia)"""
gen_variants(f"{test_ce_dir}/00_快速入門.wy", f"{ce_dir}/00_快速入門", [
    {"曰一": "曰五"},
    {"曰一": "曰四十二"},
    {"曰一": "曰一百"}
], [py00(5), py00(42), py00(100)])

# 01
def py01(jia, aiya, yy, suiyi, dayan, wenyan_str):
    yy_str = "True" if yy else "False"
    return f"""jia = {jia}
aiya = '{aiya}'
yinyang = {yy_str}
suiyi = {suiyi}
dayan = {dayan}
wenyan_str = '{wenyan_str}'

print(jia)
print(aiya)
print("陽" if yinyang else "陰")
print(suiyi)
print(dayan)
print(wenyan_str)"""
gen_variants(f"{test_ce_dir}/01_開物_定名.wy", f"{ce_dir}/01_開物_定名", [
    {"曰一。名之曰「甲」": "曰七。名之曰「甲」", "曰一。名之曰「隨義」": "曰三。名之曰「隨義」", "五十": "九十九", "「「噫吁戲」」": "「「天下太平」」", "「「問天地好在。」」": "「「萬物並作」」", "曰陰": "曰陽"},
    {"曰一。名之曰「甲」": "曰二。名之曰「甲」", "曰一。名之曰「隨義」": "曰二。名之曰「隨義」", "五十": "二十五", "「「噫吁戲」」": "「「風雲際會」」", "「「問天地好在。」」": "「「春風化雨。」」"},
    {"曰一。名之曰「甲」": "曰一百。名之曰「甲」", "曰一。名之曰「隨義」": "曰十。名之曰「隨義」", "有數五十": "有數一百二十八", "「「噫吁戲」」": "「「龍鳳呈祥」」", "「「問天地好在。」」": "「「日月星辰。」」", "曰陰": "曰陽"}
], [
    py01(7, '天下太平', True, 3, 99, '萬物並作'),
    py01(2, '風雲際會', False, 2, 25, '春風化雨。'),
    py01(100, '龍鳳呈祥', True, 10, 128, '日月星辰。')
])

# 02
def py02(jia, yi, bing, dayan):
    return f"""jia = {jia}
yi = {yi}
bing = {bing}
ding = jia
dayan = {dayan}

print(jia)
print(yi)
print(bing)
print(ding)
print(dayan)

bing = dayan
print(bing)"""
gen_variants(f"{test_ce_dir}/02_流轉_易值.wy", f"{ce_dir}/02_流轉_易值", [
    {"曰一。曰三。曰五。": "曰二。曰六。曰十。", "有數五十。": "有數一百。"},
    {"曰一。曰三。曰五。": "曰四。曰八。曰十二。", "有數五十。": "有數七十七。"},
    {"曰一。曰三。曰五。": "曰九。曰二十七。曰八十一。", "有數五十。": "有數三十六。"}
], [py02(2, 6, 10, 100), py02(4, 8, 12, 77), py02(9, 27, 81, 36)])

# 03
def py03(ji, yi, xin, ren, gui, aiya, yy, wenyan_str, dayan, suiyi):
    yy_str = "True" if yy else "False"
    return f"""jia = 0
ji = {ji}
yi = {yi}
bing = 5
xin = {xin}
ren = {ren}
gui = {gui}
aiya = '{aiya}'
yinyang = {yy_str}
suiyi = {suiyi}
dayan = {dayan}
wenyan_str = '{wenyan_str}'

bing = dayan
ding = ji

print('觀乎數理，甲得', jia, '，乙得', yi)
print('『丙』今化為大衍之數：', bing)
print('陰陽之相乃', "陽" if yinyang else "陰")
print('有言曰：' + wenyan_str + aiya)
print('辛', xin, '，壬', ren, '，癸', gui)
print('丁之值為：', ding)
print('隨義之值為：', suiyi)"""
gen_variants(f"{test_ce_dir}/03_留墨_書法.wy", f"{ce_dir}/03_留墨_書法", [
    {"曰一。名之曰「己」": "曰五。名之曰「己」", "曰三。曰五。": "曰七。曰五。", "曰二。曰四。曰六。": "曰三。曰六。曰九。", "「「噫吁戲！」」": "「「鳴鳳在樹！」」", "曰陰": "曰陽", "「「問天地好在。」」": "「「白雲蒼狗。」」", "有數五十": "有數八十八", "曰一。名之曰「隨義」": "曰二。名之曰「隨義」"},
    {"曰一。名之曰「己」": "曰三。名之曰「己」", "曰三。曰五。": "曰九。曰五。", "曰二。曰四。曰六。": "曰一。曰二。曰三。", "「「噫吁戲！」」": "「「海晏河清。」」", "曰陰": "曰陰", "「「問天地好在。」」": "「「山高水長。」」", "有數五十": "有數六十四", "曰一。名之曰「隨義」": "曰四。名之曰「隨義」"},
    {"曰一。名之曰「己」": "曰八。名之曰「己」", "曰三。曰五。": "曰四。曰五。", "曰二。曰四。曰六。": "曰五。曰十。曰十五。", "「「噫吁戲！」」": "「「金玉滿堂！」」", "曰陰": "曰陽", "「「問天地好在。」」": "「「天長地久。」」", "有數五十": "有數二百", "曰一。名之曰「隨義」": "曰七。名之曰「隨義」"}
], [
    py03(5, 7, 3, 6, 9, '鳴鳳在樹！', True, '白雲蒼狗。', 88, 2),
    py03(3, 9, 1, 2, 3, '海晏河清。', False, '山高水長。', 64, 4),
    py03(8, 4, 5, 10, 15, '金玉滿堂！', True, '天長地久。', 200, 7)
])

# 04
def py04(a, b):
    return f"""jia = {a}
yi = {b}
print('今有甲數為', jia)
print('又有乙數為', yi)
print('甲乙相并，厥和乃為', jia+yi)
print('以乙損甲，其差為', jia-yi)
print('以甲損乙，反得其差為', yi-jia)
print('甲乙相乘，步其積為', jia*yi)
print('以乙除甲，得商', jia//yi, '，不盡者為', jia%yi)
jia += 1
yi -= 1
print('甲益一算，厥數乃為', jia)
print('乙損一籌，其數化為', yi)
wu = jia + yi + 10
print('今以甲數', jia, '，并乙復益十，步之得', wu, '，與戊同揆，皆為', wu)
print('并甲於乙，其數為', jia+yi)
print('乘甲以乙，其積為', jia*yi)"""
gen_variants(f"{test_ce_dir}/04_九章_算術.wy", f"{ce_dir}/04_九章_算術", [
    {"曰十。曰三。": "曰八。曰五。"},
    {"曰十。曰三。": "曰十五。曰四。"},
    {"曰十。曰三。": "曰六。曰七。"}
], [py04(8, 5), py04(15, 4), py04(6, 7)])

# 05
def py05(add1, add2, add3, add4, add5):
    return f"""print('老夫今日不卜卦，且出城踏青，算算這山間野趣。')
print('一去二三里，煙村四五家。')
lu = 2
lu += {add1}
cun = 4
cun += {add2}
print('信步走來，行了', lu, '里路，遇見幾戶人家？想來是', cun, '戶矣。')
print('亭臺六七座，八九十枝花。')
ting = 6
ting += {add3}
hua = 8
hua += {add4}
hua += {add5}
print('歇腳之處，竟有', ting, '座之多。')
print('路邊繁花更盛，總計', hua, '枝！')
wanwu = lu + cun + ting + hua
print('老夫掐指一總，路、村、亭、花，此行共覽風物', wanwu, '件。想當然耳，風雅竟能稱斤論兩，妙極妙極！')"""
gen_variants(f"{test_ce_dir}/05_山村_算術.wy", f"{ce_dir}/05_山村_算術", [
    {"加三於「去路」": "加一於「去路」", "加五於「人家」": "加一於「人家」", "加七於「亭臺」": "加一於「亭臺」", "加九於「繁花」": "加一於「繁花」", "加十於「繁花」": "加一於「繁花」"},
    {"加三於「去路」": "加十於「去路」", "加五於「人家」": "加十於「人家」", "加七於「亭臺」": "加十於「亭臺」", "加九於「繁花」": "加十於「繁花」", "加十於「繁花」": "加十於「繁花」"},
    {"加三於「去路」": "加五於「去路」", "加五於「人家」": "加五於「人家」", "加七於「亭臺」": "加五於「亭臺」", "加九於「繁花」": "加五於「繁花」", "加十於「繁花」": "加五於「繁花」"}
], [py05(1, 1, 1, 1, 1), py05(10, 10, 10, 10, 10), py05(5, 5, 5, 5, 5)])

# 06
def py06(n):
    return f"""print('松下問童子，欲尋隱者蹤。')
xiansuo = {n}
if xiansuo > 2:
    print('想當然耳！線索俱全，必能尋得隱者。')
else:
    print('怪哉！深山逢童子，竟問不出隻言片語？')

if xiansuo == 1:
    print('童子答曰：『言師採藥去。』')
elif xiansuo == 2:
    print('童子指山曰：『只在此山中。』')
elif xiansuo == 3:
    print('童子搖頭曰：『雲深不知處。』')
else:
    print('奇了，童子竟言他物！')

print('妙哉，山中尚存一絲雲霧，仙氣縹緲！')
print('大善！雲遮霧掩，今皆化作漫天純白，真乃『雲深不知處』矣！')"""
gen_variants(f"{test_ce_dir}/06_尋隱_決策.wy", f"{ce_dir}/06_尋隱_決策", [
    {"有數三。名之曰「尋訪線索」。": "有數一。名之曰「尋訪線索」。"},
    {"有數三。名之曰「尋訪線索」。": "有數二。名之曰「尋訪線索」。"},
    {"有數三。名之曰「尋訪線索」。": "有數四。名之曰「尋訪線索」。"}
], [py06(1), py06(2), py06(4)])

# 07
def py07(v1, v2):
    return f"""print('春日宴。')
jiu_shu = {v1}
while jiu_shu != 0:
    print('飲綠酒', jiu_shu, '杯，高歌一遍。')
    jiu_shu -= 1

print('酒闌之際，再拜陳三願。')
for _ in range(2):
    print('一願郎君千歲，二願妾身常健。')

yuan_shu = {v2}
if yuan_shu > 2:
    for _ in range(2):
        print('三願如同梁上燕，歲歲長相見。')
else:
    for _ in range(3):
        print('怪哉！願數不足，花落人散兩成空！')
"""
gen_variants(f"{test_ce_dir}/07_酒宴_循環.wy", f"{ce_dir}/07_酒宴_循環", [
    {"有數三。名之曰「酒數」。": "有數二。名之曰「酒數」。", "有數三。名之曰「願數」。": "有數三。名之曰「願數」。"},
    {"有數三。名之曰「酒數」。": "有數五。名之曰「酒數」。", "有數三。名之曰「願數」。": "有數一。名之曰「願數」。"},
    {"有數三。名之曰「酒數」。": "有數四。名之曰「酒數」。", "有數三。名之曰「願數」。": "有數五。名之曰「願數」。"}
], [py07(2, 3), py07(5, 1), py07(4, 5)])

# 08
v8_py = """jia = 1
while jia < 10:
    yi = 1
    while yi < 10:
        guo = jia * yi
        if guo >= 10:
            print(jia, yi, guo)
        else:
            print(jia, yi, '得', guo)
        yi += 1
    jia += 1
"""
gen_variants(f"{test_ce_dir}/08_口訣_乘算.wy", f"{ce_dir}/08_口訣_乘算", [{}, {}, {}], [v8_py]*3)

# 09
v9_py = """print('今有雞翁一，值錢五；雞母一，值錢三；雞雛三，值錢一。')
print('凡百錢買雞百隻，問雞翁、母、雛各幾何？')
bai = 100
weng = 0
while weng <= 20:
    mu = 0
    while mu <= 33:
        chu = bai - weng - mu
        chu_yu = chu % 3
        if chu_yu == 0:
            weng_jia = weng * 5
            mu_jia = mu * 3
            chu_jia = chu // 3
            zong_jia = weng_jia + mu_jia + chu_jia
            if zong_jia == bai:
                print('得解：翁', weng, '隻，母', mu, '隻，雛', chu, '隻。')
        mu += 1
    weng += 1
"""
gen_variants(f"{test_ce_dir}/09_百雞_算術.wy", f"{ce_dir}/09_百雞_算術", [{}, {}, {}], [v9_py]*3)

# 10
def py10(text, length):
    return f"""zheng = "{text}"
chang = {length}
dao = ""
jia = 0
while jia != chang:
    suoyin = chang - jia - 1
    dan = zheng[suoyin]
    dao += dan
    jia += 1
print('正向觀之：' + zheng)
print('倒向讀之：' + dao)"""
gen_variants(f"{test_ce_dir}/10_客上_倒敘.wy", f"{ce_dir}/10_客上_倒敘", [
    {"「「客上天然居」」": "「「山高水長」」", "有數五": "有數四"},
    {"「「客上天然居」」": "「「天地玄黃」」", "有數五": "有數四"},
    {"「「客上天然居」」": "「「風花雪月」」", "有數五": "有數四"}
], [
    py10("山高水長", 4),
    py10("天地玄黃", 4),
    py10("風花雪月", 4)
])

# 11
def py11(w1, w2):
    return f"""wu_yin = {w1}
si_xiang = {w2}
tian_di = [wu_yin, si_xiang]

print('五音之長，計有：')
print(len(wu_yin))
print('四象之長，計有：')
print(len(si_xiang))
print('天地萬象之長（內含幾列），計有：')
print(len(tian_di))

print('以迴圈逐一取五音列中之物：')
jia = 1
while jia <= len(wu_yin):
    print(wu_yin[jia-1])
    jia += 1

print('以迴圈逐一取四象列中之物：')
yi = 1
while yi <= len(si_xiang):
    print(si_xiang[yi-1])
    yi += 1"""
gen_variants(f"{test_ce_dir}/11_布陣_行列.wy", f"{ce_dir}/11_布陣_行列", [
    {"「「宮」」": "「「金」」", "「「商」」": "「「木」」", "「「角」」": "「「水」」", "「「徵」」": "「「火」」", "「「羽」」": "「「土」」",
     "「「青龍」」": "「「春」」", "「「白虎」」": "「「夏」」", "「「朱雀」」": "「「秋」」", "「「玄武」」": "「「冬」」"},
    {"「「宮」」": "「「甲」」", "「「商」」": "「「乙」」", "「「角」」": "「「丙」」", "「「徵」」": "「「丁」」", "「「羽」」": "「「戊」」",
     "「「青龍」」": "「「子」」", "「「白虎」」": "「「丑」」", "「「朱雀」」": "「「寅」」", "「「玄武」」": "「「卯」」"},
    {"「「宮」」": "「「一」」", "「「商」」": "「「二」」", "「「角」」": "「「三」」", "「「徵」」": "「「四」」", "「「羽」」": "「「五」」",
     "「「青龍」」": "「「東」」", "「「白虎」」": "「「西」」", "「「朱雀」」": "「「南」」", "「「玄武」」": "「「北」」"}
], [
    py11(["金", "木", "水", "火", "土"], ["春", "夏", "秋", "冬"]),
    py11(["甲", "乙", "丙", "丁", "戊"], ["子", "丑", "寅", "卯"]),
    py11(["一", "二", "三", "四", "五"], ["東", "西", "南", "北"])
])

# 12 萬化方術
def py12(vals):
    out = ""
    for v in vals:
        out += f"""def qiu_gen_{v}(n):
    g = 1
    for _ in range(n):
        yi = g * g - 2
        bing = 2 * g
        mao = int(yi / bing)
        g = g - mao
    return g
print(qiu_gen_{v}({v}))
"""
    return out

gen_variants(f"{test_ce_dir}/12_萬化_方術.wy", f"{ce_dir}/12_萬化_方術", [
    {"於二。": "於四。", "於三。": "於六。", "於五。": "於十。", "於七。": "於十四。", "於十三。": "於二十六。", "於一百三十一。": "於二百。", "於一千零一十三。": "於二千。"},
    {"於二。": "於八。", "於三。": "於九。", "於五。": "於十五。", "於七。": "於二十。", "於十三。": "於三十。", "於一百三十一。": "於三百。", "於一千零一十三。": "於三千。"},
    {"於二。": "於十。", "於三。": "於十二。", "於五。": "於二十。", "於七。": "於三十。", "於十三。": "於四十。", "於一百三十一。": "於四百。", "於一千零一十三。": "於四千。"}
], [
    py12([4, 6, 10, 14, 26, 200, 2000]),
    py12([8, 9, 15, 20, 30, 300, 3000]),
    py12([10, 12, 20, 30, 40, 400, 4000])
])

# 割圓術
def circle_py(n):
    return f"""def qiu_xiao_xian(li_gu_mian, lun_shu):
    ban_mian = li_gu_mian / 2.0
    if lun_shu < 1.0: return li_gu_mian
    ban_mian_sq = ban_mian * ban_mian
    gou = (1.0 - ban_mian_sq)**0.5
    xiao_gou = 1.0 - gou
    xiao_xian = (xiao_gou * xiao_gou + ban_mian_sq)**0.5
    return qiu_xiao_xian(xiao_xian, lun_shu - 1.0)

def ge_yuan(yuan_lun):
    lun_shu = yuan_lun - 1.0
    gu_shu = (2.0 ** lun_shu) * 6.0
    xiao_xian = qiu_xiao_xian(1.0, lun_shu)
    gu_zhou = xiao_xian * gu_shu
    gu_mi = (gu_zhou / 2.0) * 1.0
    yuan_lu = gu_mi / 1.0
    print('割圓', int(gu_shu), '觚。所得圓率者。')
    print(f"{{yuan_lu:.6g}}", '也。')

ge_yuan({n})"""
gen_variants(f"{test_dian_dir}/割圓術.wy", f"{dian_dir}/割圓術", [
    {"於十。": "於五。"}, {"於十。": "於八。"}, {"於十。": "於十二。"}
], [circle_py(5), circle_py(8), circle_py(12)])

# 曼德博集
def mandel_py(w, h):
    return f"""shan_fa = '丁龘蠹臺龜畫龍淼蔑高五三二'
shang = -1.0; xia = 1.0
zuo = -2.0; you = 2.0
heng = (you - zuo) / {w}
zong = (xia - shang) / {h}

wu = 0
while wu < {h}:
    xu = zong * wu + shang
    hang = ''
    xu2 = 0
    while xu2 < {w}:
        shi = heng * xu2 + zuo
        xu_xu = xu
        shi_shi = shi
        ji = 0
        while ji < 12:
            jia = shi_shi * shi_shi
            yi = xu_xu * xu_xu
            bing = jia + yi
            if bing > 4.0:
                break
            xu_xu = xu_xu * shi_shi * 2.0 + xu
            shi_shi = jia - yi + shi
            ji += 1
        
        si = 13 - ji
        mo = shan_fa[si-1]
        hang += mo
        xu2 += 1
    print(hang)
    wu += 1"""
gen_variants(f"{test_dian_dir}/曼德博集.wy", f"{dian_dir}/曼德博集", [
    {"於五十九。於二十四。": "於四十。於二十。"},
    {"於五十九。於二十四。": "於三十。於十五。"},
    {"於五十九。於二十四。": "於二十。於一十。"}
], [mandel_py(40, 20), mandel_py(30, 15), mandel_py(20, 10)])

# 牛頓求根法
def newton_py(n):
    return f"""def qiu_gen(n):
    g = 1.0
    for _ in range({n}):
        yi = g * g - 2.0
        bing = 2.0 * g
        mao = yi / bing
        g = g - mao
    return g
print(f"{{qiu_gen({n}):.6g}}")"""
gen_variants(f"{test_dian_dir}/牛頓求根法.wy", f"{dian_dir}/牛頓求根法", [
    {"於一千零一十三。": "於二十。"},
    {"於一千零一十三。": "於五十。"},
    {"於一千零一十三。": "於九十九。"}
], [newton_py(20), newton_py(50), newton_py(99)])

write_sh(f"{base_dir}/run_extra_tests.sh")
