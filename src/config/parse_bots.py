import re
import sys
import yaml_custom as yaml
import base64
from PIL import Image
from cpp_generator import print_cpp_file, CppEnum, CppObject, CppLiteral

PROPIC_SIZE = 50
INCLUDE_FILENAMES = ['net/bot_info.h']
OBJECT_DECLARATION = 'bot_info_t banggame::bot_info'

def sdl_image_pixels(filename):
    with Image.open(filename) as image:
        w = image.width
        h = image.height

        if w > h:
            h = PROPIC_SIZE * h // w
            w = PROPIC_SIZE
        else:
            w = PROPIC_SIZE * w // h
            h = PROPIC_SIZE

        pixels = base64.b64encode(image.resize((w, h)).convert('RGBA').tobytes('raw', 'RGBA', 0, 1)).decode('utf8')
        
        return CppObject(
            width = w,
            height = h,
            pixels = CppLiteral(f'base64::base64_decode("{pixels}")')
        )
    
def parse_bot_rule(value):
    match = re.match(
        r'^\s*(\w+)' # rule_name
        r'(?:\s*\((.+)\))?\s*$', # rule_args
        value
    )
    if not match:
        raise RuntimeError(f'Invalid rule string: {value}')
    
    rule_name = match.group(1)
    rule_args = match.group(2)
    if rule_args:
        return CppLiteral(f'BUILD_BOT_RULE({rule_name}, {rule_args})')
    else:
        return CppLiteral(f'BUILD_BOT_RULE({rule_name})')

def parse_settings(settings):
    return CppObject(
        allow_timer_no_action = settings['allow_timer_no_action'],
        max_random_tries = settings['max_random_tries'],
        bypass_prompt_after = settings['bypass_prompt_after'],
        response_rules = [parse_bot_rule(value) for value in settings['response_rules']],
        in_play_rules = [parse_bot_rule(value) for value in settings['in_play_rules']]
    )

def parse_file(data):
    return CppObject(
        names = data['names'],
        propics = [sdl_image_pixels(filename) for filename in data['propics']],
        settings = parse_settings(data['settings'])
    )

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(f'Usage: {sys.argv[0]} bot_info.yml bot_info.cpp')
        sys.exit(1)

    with open(sys.argv[1], 'r', encoding='utf8') as file:
        bot_info = parse_file(yaml.safe_load(file)['bots'])
    
    if sys.argv[2] == '-':
        print_cpp_file(bot_info, OBJECT_DECLARATION,
            include_filenames=INCLUDE_FILENAMES,
            file=sys.stdout)
    else:
        with open(sys.argv[2], 'w', encoding='utf8') as file:
            print_cpp_file(bot_info, OBJECT_DECLARATION,
                include_filenames=INCLUDE_FILENAMES,
                file=file)