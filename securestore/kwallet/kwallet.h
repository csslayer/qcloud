#ifndef KWALLET_H
#define KWALLET_H

#include "isecurestore.h"
#include <KWallet/Wallet>

using KWallet::Wallet;

namespace QCloud{

enum KWALLET_STAT {NOT_SET,SUCCEEDED,FAILED,NOT_AVAILABLE};

class KWalletStore :  public ISecureStore
{
    Q_OBJECT;
public:
    
    KWalletStore();
    
    bool isAvailable();
    
    bool GetItem(const QString& key,QString& value);
    bool SetItem(const QString& key,const QString& value);
    
    private slots:
        void walletOpened(bool success);
    
private:
    Wallet *m_wallet;
    KWALLET_STAT stat;
};

}

#endif