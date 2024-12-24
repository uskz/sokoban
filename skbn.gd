extends Node

# プレイヤー座標情報
var j = Vector2(0, 0)
# 移動方向情報
var d = Vector2(0, 0)
# 現在のステージ数
var lv = 0
# ステージクリアフラグ
var bClear = false
# 現在のステージデータ
var Map = []
# 全ステージデータ
var Dat = []
# マッピング
var M = {
	"NONE": " ",
	"WALL": "#",
	"NI": "o",
	"NI_ON": "O",
	"SELF": "t",
	"SELF_ON": "T",
	"GOAL": ".",
	"RTN": "\n",
	"END": "E"
}
# 入力ディレイ関連
var input_power = 0

func _init():
	# 最初のステージ枠確保
	Dat.append([])
	# ステージデータファイル読み込み
	var file = FileAccess.open("res://dat_skbn.txt", FileAccess.READ)
	if file:
		var content = file.get_as_text()
		for t in content:
			if t == M["END"]:
				Dat.append([])
			else:
				Dat[Dat.size() - 1].append(t)
		file.close()
	InitSelfPos()  # プレイヤー位置初期化
	Draw()  # 画面描画

func IsAllClear() -> bool:
	if bClear:
		if (lv + 1 != Dat.size()):
			lv += 1
			bClear = false
			InitSelfPos()
			Draw()
	return bClear

func Inputkey():
	if Input.is_action_pressed("ui_up"):
		d = Vector2(0, -1)
	elif Input.is_action_pressed("ui_down"):
		d = Vector2(0, 1)
	elif Input.is_action_pressed("ui_right"):
		d = Vector2(1, 0)
	elif Input.is_action_pressed("ui_left"):
		d = Vector2(-1, 0)
	else:
		input_power = 0
	
	if input_power > 7:
		input_power = 0
	else:
		input_power += 1
		d = Vector2(0, 0)

func Update():
	var t0 = Pos(j.x, j.y)
	var t1 = Pos(j.x + d.x, j.y + d.y)
	if Map[t1] in [M["GOAL"], M["NONE"]]:
		Map[t1] = M["SELF"] if Map[t1] == M["NONE"] else M["SELF_ON"]
		Map[t0] = M["NONE"] if Map[t0] == M["SELF"] else M["GOAL"]
	elif Map[t1] in [M["NI"], M["NI_ON"]]:
		var t2 = Pos(j.x + d.x * 2, j.y + d.y * 2)
		if Map[t2] in [M["GOAL"], M["NONE"]]:
			Map[t2] = M["NI"] if Map[t2] == M["NONE"] else M["NI_ON"]
			Map[t1] = M["SELF"] if Map[t1] == M["NI"] else M["SELF_ON"]
			Map[t0] = M["NONE"] if Map[t0] == M["SELF"] else M["GOAL"]
	if Map[t0] not in [M["SELF"], M["SELF_ON"]]:
		j = Vector2(j.x + d.x, j.y + d.y)
		bClear = not M["NI"] in Map

func Draw():
	var children = get_children()
	var idx = 0
	for m in Map:
		if m != M["RTN"]:
			children[idx].text = m
			idx += 1

func InitSelfPos():
	var c = Vector2(0, 0)
	j = Vector2(0, 0)
	Map = Dat[lv]
	for child in get_children():
		child.queue_free()
	for m in Map:
		if m != M["RTN"]:
			var chip = Label.new()
			chip.text = m
			chip.horizontal_alignment = HORIZONTAL_ALIGNMENT_CENTER
			chip.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
			chip.position = c
			add_child(chip)
			c = Vector2(c.x + 13, c.y)
		else:
			c = Vector2(0, c.y + 13)
	for m in Map:
		if m in [M["SELF"], M["SELF_ON"]]:
			break
		elif m == M["RTN"]:
			j = Vector2(0, j.y + 1)
		else:
			j.x += 1

func Pos(x: int, y: int) -> int:
	var i = 0
	var ycnt = 0
	for m in Map:
		if ycnt == y:
			break
		if m == M["RTN"]:
			ycnt += 1
		i += 1
	return i + x

func _process(_delta):
	if not IsAllClear():
		Inputkey()
		Update()
		Draw()
