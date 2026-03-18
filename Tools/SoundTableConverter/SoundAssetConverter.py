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
                continue

            entry = {}

            for k, v in row.items():
                if k == "Key":
                    continue

                if v is None or v == "":
                    entry[k] = v
                    continue

                v_lower = v.lower()

                if v_lower in ("true", "1"):
                    entry[k] = True
                elif v_lower in ("false", "0"):
                    entry[k] = False
                else:
                    try:
                        if "." in v:
                            entry[k] = float(v)
                        else:
                            entry[k] = int(v)
                    except ValueError:
                        entry[k] = v

            sound_dict[key] = entry

    final_json = {"Descriptors": sound_dict}

    with open(json_path, "w", encoding="utf-8") as f:
        json.dump(final_json, f, ensure_ascii=False, indent=4)

    print(f"JSON 출력 완료: {json_path}")


if __name__ == "__main__":

    tables = [
        ("StaticSoundDescriptors.csv", "StaticSoundDescriptors.json"),
        ("StreamSoundDescriptors.csv", "StreamSoundDescriptors.json"),
    ]

    for csv_file, json_file in tables:
        if os.path.exists(csv_file):
            csv_to_json(csv_file, json_file)
        else:
            print("CSV 파일이 없습니다:", csv_file)
