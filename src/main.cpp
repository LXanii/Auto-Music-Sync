#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// OBJECT IDS | slow = 200 0.7| normal speed = 201, 0.9 | 2x = 202 1.1| 3x = 203 1.3| 4x = 1334 1.6| m_levelString

std::vector<int> slowSpeed;
std::vector<int> normalSpeed;
std::vector<int> fastSpeed;
std::vector<int> fasterSpeed;
std::vector<int> fastestSpeed;

std::vector<std::string> splitString(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream ss(s);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token); 
    }
    return tokens;
}

void cleanVector(std::vector<int>& vector) {
	std::sort(vector.begin(), vector.end());
	auto uniquePositions = std::unique(vector.begin(), vector.end());
	vector.erase(uniquePositions, vector.end());
}

class $modify(ResetMusicLayer, PlayLayer) {

float pitch = 1.0f;
float supposedSpeed;
bool needingOfPitch = false;

	bool init(GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
		bool result = PlayLayer::init(level, useReplay, dontCreateObjects);
		m_fields->supposedSpeed = 0.0f;
		auto fmod = FMODAudioEngine::sharedEngine();
		FMOD_RESULT fResult;
		FMOD::ChannelGroup* mGroup = nullptr;
		fResult = fmod->m_system->getMasterChannelGroup(&mGroup);
		ResetMusicLayer::quickRestart();
		gd::string levelString = m_level->m_levelString;
		std::string decompString = ZipUtils::decompressString(levelString, true, 0);
		std::vector<std::string> leveldata = splitString(decompString, ';');
		slowSpeed.clear(); normalSpeed.clear(); fastSpeed.clear(); fasterSpeed.clear(); fastestSpeed.clear(); // real ones use 1 line to save 5

		m_fields->supposedSpeed = 0;
		if (m_fields->pitch != 1.0f) mGroup->setPitch(1.0f);
		int pixelBuffer = Mod::get()->getSettingValue<int64_t>("pixelBuffer");

		for (int i = 0; i < leveldata.size(); i++) {
			if (leveldata[i].substr(0, 6) == "1,200,") { 
					std::vector<std::string> objectXPOS = splitString(leveldata[i], ','); 
					slowSpeed.push_back(stoi(objectXPOS[3]) - pixelBuffer); 
				}
			else if (leveldata[i].substr(0, 6) == "1,201,") { 
					std::vector<std::string> objectXPOS = splitString(leveldata[i], ','); 
					normalSpeed.push_back(stoi(objectXPOS[3]) - pixelBuffer);
				}
			else if (leveldata[i].substr(0, 6) == "1,202,") { 
					std::vector<std::string> objectXPOS = splitString(leveldata[i], ',');
					fastSpeed.push_back(stoi(objectXPOS[3]) - pixelBuffer); 
				}
			else if (leveldata[i].substr(0, 6) == "1,203,") { 
					std::vector<std::string> objectXPOS = splitString(leveldata[i], ',');
					fasterSpeed.push_back(stoi(objectXPOS[3]) - pixelBuffer); 
				}
			else if (leveldata[i].substr(0, 7) == "1,1334,")  { 
					std::vector<std::string> objectXPOS = splitString(leveldata[i], ','); 
					fastestSpeed.push_back(stoi(objectXPOS[3]) - pixelBuffer); // 40 is accounted for the start of the hitbox
				}	
		}
		cleanVector(slowSpeed); cleanVector(normalSpeed); cleanVector(fastSpeed); cleanVector(fasterSpeed); cleanVector(fastestSpeed);
		log::info("{} {} {} {} {}", slowSpeed, normalSpeed, fastSpeed, fasterSpeed, fastestSpeed);
		return result;
	}

	void postUpdate(float p0) {
		PlayLayer::postUpdate(p0);
		auto fmod = FMODAudioEngine::sharedEngine();
		FMOD_RESULT result;
		FMOD::ChannelGroup* mGroup = nullptr;
		result = fmod->m_system->getMasterChannelGroup(&mGroup);
		if (m_fields->supposedSpeed == 0.0f) m_fields->supposedSpeed = m_player1->m_playerSpeed; // YEAH BABY INIT WOOO
		if (m_fields->needingOfPitch) {
			for (int i = 0; i < slowSpeed.size(); i++) {
				if (std::abs(floor(m_player1->getPositionX()) - slowSpeed[i]) <= 5) {
					log::debug("{} {} 0.7 {}", floor(m_player1->getPositionX()), m_player1->m_playerSpeed, slowSpeed[i]);
					m_fields->supposedSpeed = 0.7;
					m_fields->pitch = m_player1->m_playerSpeed / 0.7;
				}
			}
			for (int i = 0; i < normalSpeed.size(); i++) {
				if (std::abs(floor(m_player1->getPositionX()) - normalSpeed[i]) <= 5) {
					log::debug("{} {} 0.9 {}", floor(m_player1->getPositionX()), m_player1->m_playerSpeed, normalSpeed[i]);
					m_fields->supposedSpeed = 0.9;
					m_fields->pitch = m_player1->m_playerSpeed / 0.9;
				}
			}
			for (int i = 0; i < fastSpeed.size(); i++) {
				if (std::abs(floor(m_player1->getPositionX()) - fastSpeed[i]) <= 5) {
					log::debug("{} {} 1.1 {}", floor(m_player1->getPositionX()), m_player1->m_playerSpeed, fastSpeed[i]);
					m_fields->supposedSpeed = 1.1;
					m_fields->pitch = m_player1->m_playerSpeed / 1.1;
				}
			}
			for (int i = 0; i < fasterSpeed.size(); i++) {
				if (std::abs(floor(m_player1->getPositionX()) - fasterSpeed[i]) <= 5) {
					log::debug("{} {} 1.3 {}", floor(m_player1->getPositionX()), m_player1->m_playerSpeed, fasterSpeed[i]);
					m_fields->supposedSpeed = 1.3;
					m_fields->pitch = m_player1->m_playerSpeed / 1.3;
				}
			}
			for (int i = 0; i < fastestSpeed.size(); i++) {
				if (std::abs(floor(m_player1->getPositionX()) - fastestSpeed[i]) <= 5) {
					log::debug("{} {} 1.6 {}", floor(m_player1->getPositionX()), m_player1->m_playerSpeed, fastestSpeed[i]);
					m_fields->supposedSpeed = 1.6;
					m_fields->pitch = m_player1->m_playerSpeed / 1.6;
				}
			}
			auto fmod = FMODAudioEngine::sharedEngine();
			if (!m_player1->m_isPlatformer) {
				if (m_player1->m_playerSpeed != m_fields->supposedSpeed) {
					mGroup->setPitch(m_fields->pitch);
				}
				else if (m_player1->m_playerSpeed == m_fields->supposedSpeed) mGroup->setPitch(1.0f);
			}
		}
	}
	
	void updateAttempts() {
		PlayLayer::updateAttempts();
		ResetMusicLayer::resetMusic();
	}

	void onExit() {
		PlayLayer::onExit();
		ResetMusicLayer::resetMusic();
	}

	void resetLevel() {
		PlayLayer::resetLevel();
		ResetMusicLayer::resetMusic();
		ResetMusicLayer::quickRestart();
	}

	void levelComplete() {
		PlayLayer::levelComplete();
		ResetMusicLayer::resetMusic();
	}

	void resetMusic() {
		m_fields->needingOfPitch = false;
		auto fmod = FMODAudioEngine::sharedEngine();
		FMOD_RESULT result;
		FMOD::ChannelGroup* mGroup = nullptr;
		result = fmod->m_system->getMasterChannelGroup(&mGroup);
		mGroup->setPitch(1.0f);
	}

	void quickRestart() {
		m_fields->supposedSpeed = 0.0f;
		auto fmod = FMODAudioEngine::sharedEngine();
		FMOD_RESULT fResult;
		FMOD::ChannelGroup* mGroup = nullptr;
		fResult = fmod->m_system->getMasterChannelGroup(&mGroup);
		m_fields->needingOfPitch = false;
		mGroup->setPitch(1.0f);
		m_fields->needingOfPitch = true;
	}
};

// todo for next update | make it work with arrow trigger sad arrow trigger