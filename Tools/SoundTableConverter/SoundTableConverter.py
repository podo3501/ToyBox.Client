import csv
import json
import os

def csv_to_json(csv_path, json_path):
    sound_dict = {}

    with open(csv_path, "r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row in reader:
            key = row.get("Key")
            if not key:
                continue  # Key가 없으면 스킵

            # 숫자형 필드 변환
            entry = {}
            for k, v in row.items():
                if k == "Key":
                    continue  # Key는 딕셔너리 키로 사용
                try:
                    if v and "." in v:
                        entry[k] = float(v)
                    elif v:
                        entry[k] = int(v)
                    else:
                        entry[k] = v
                except:
                    entry[k] = v

            sound_dict[key] = entry

    # 최상위 키 "SoundInfo"로 감싸기
    final_json = {"Infos": sound_dict}

    with open(json_path, "w", encoding="utf-8") as f:
        json.dump(final_json, f, ensure_ascii=False, indent=4)

    print(f"JSON 출력 완료: {json_path}")


if __name__ == "__main__":
    csv_file = "SoundTable.csv"
    json_file = "SoundTable.json"

    if os.path.exists(csv_file):
        csv_to_json(csv_file, json_file)
    else:
        print("CSV 파일이 없습니다:", csv_file)
