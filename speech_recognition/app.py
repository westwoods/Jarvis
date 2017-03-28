#-*- coding: utf-8 -*-
# Created by JHJ on 2017. 2. 14.

if __name__ == '__main__':
    import speech_recognition as sr
    from utils.my_requests import get_news
    from utils.read import reading
    from konlpy.tag import Twitter

    twitter = Twitter()

    # Record Audio
    r = sr.Recognizer()
    with sr.Microphone() as source:
        print "말해보세요!"
        audio = r.listen(source)

    try:
        speech_string = r.recognize_google(audio, language='ko-KR')
        print(speech_string)
        print(speech_string.replace(' ', ''))
        morphemes = twitter.nouns(speech_string.replace(' ', ''))
        print(morphemes)

        if u'뉴스' in morphemes:
            data = get_news()
            reading(data)

    except sr.UnknownValueError:
        print "Google Speech Recognition could not understand audio"
    except sr.RequestError as e:
        print "Could not request results from Google Speech Recognition service; {0}".format(e)
